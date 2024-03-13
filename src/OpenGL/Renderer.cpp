#include "Renderer.hpp"
#include "OpenGL/Buffer.hpp"
#include "OpenGL/Error.hpp"
#include "Window.hpp"
#include <GL/glew.h>
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace
{
const char* scalingVertexShader{ R"""(#version 330 core
layout(location = 0) in vec2 position2d;
layout(location = 1) in vec3 color;
out vec3 colorPS;
uniform mat3 inputTransform;
void main() {
  vec3 transformed = inputTransform * vec3(position2d, 1.f);
  gl_Position = vec4(transformed.xy / transformed.z, 0.f, 1.f);
  colorPS = color;
}
)""" };

const char* passthroughFragmentShader{ R"""(#version 330 core
in vec3 colorPS;
layout(location = 0) out vec3 colorOut;
void main() {
  colorOut = colorPS;
}
)""" };
}

namespace gl
{
Renderer::Renderer(Window& window, const Vector2& dpi)
  : m_dpi(dpi)
  , m_program(scalingVertexShader, passthroughFragmentShader)
{
  CheckError();

  glEnable(GL_MULTISAMPLE);
  glDisable(GL_DEPTH_TEST);

  window.SetMouseMoveCallback([this](const Vector2i& position)
  {
    if (m_leftButtonPressed)
    {
      Vector2 normalizedMousePosition{ GetNormalizedMousePosition(position) };
      Vector2 movement{ m_lastMousePosition - normalizedMousePosition };
      float zoom{ std::pow(ZOOM_BASE, static_cast<float>(m_zoomLevel)) };
      m_pan += movement * (2.f / zoom);
      m_lastMousePosition = normalizedMousePosition;
      m_drawAreaChanged = true;
    }
  });
  window.SetMouseButtonCallback(
    [this](MouseEvents::MouseButton button, MouseEvents::MouseAction action, const Vector2i& position)
  {
    if (button == MouseEvents::MouseButton::Left)
    {
      if (action == MouseEvents::MouseAction::Press)
      {
        m_leftButtonPressed = true;
        m_lastMousePosition = GetNormalizedMousePosition(position);
      }
      else
      {
        m_leftButtonPressed = false;
      }
    }
  });
  window.SetMouseWheelHandler([this](int offset, const Vector2i& mousePosition)
  {
    int scrollDirection{ offset > 0 ? 1 : -1 };
    Vector2 normalizedMousePosition{ GetNormalizedMousePosition(mousePosition) - 0.5f };
    while (offset != 0)
    {
      int previousZoomLevel{ m_zoomLevel };
      m_zoomLevel = std::clamp(m_zoomLevel + scrollDirection, MIN_ZOOM_LEVEL, MAX_ZOOM_LEVEL);

      if (previousZoomLevel != m_zoomLevel)
      {
        float zoom{ std::pow(ZOOM_BASE, static_cast<float>(scrollDirection == 1 ? m_zoomLevel : m_zoomLevel + 1)) };
        m_pan += normalizedMousePosition.cwiseQuotient(m_dpi) / (zoom * 2.f * static_cast<float>(scrollDirection));
        m_drawAreaChanged = true;
      }

      offset -= scrollDirection;
    }
  });
}

Renderer::~Renderer()
{
  CheckError();
}

void Renderer::SetTriangleBuffer(std::vector<Triangle>&& triangles)
{
  m_triangles = triangles;
  m_ready = true;
}

void Renderer::AddTriangles(const std::vector<Triangle>& triangles)
{
  m_triangles.insert(m_triangles.end(), triangles.begin(), triangles.end());
}

void Renderer::Finish()
{
  m_ready = true;
}

void Renderer::SetWindowSize(const Vector2i& windowSize)
{
  m_windowSize = windowSize;
  m_windowSizeChanged = true;
}

void Renderer::SetDrawArea(const Rectangle& drawArea)
{
  m_drawArea = drawArea;
  m_drawAreaChanged = true;
}

void Renderer::Draw()
{
  if (!m_ready)
    return;

  if (m_initialDraw)
  {
    m_initialDraw = false;

    m_vao.Bind();

    Buffer buffer;
    buffer.Bind();
    buffer.SetData(m_triangles.size() * sizeof(m_triangles[0]), m_triangles.data());
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
      0, 2, GL_FLOAT, GL_FALSE, sizeof(Triangle::Vertex), (void*)offsetof(Triangle::Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Triangle::Vertex), (void*)offsetof(Triangle::Vertex, color));

    m_vao.Unbind();

    glClearColor(0.9f, 0.9f, 0.9f, 1.f);
    CheckError();
  }

  if (m_windowSizeChanged)
  {
    RecreateFramebuffer();
    glViewport(0, 0, m_windowSize.x, m_windowSize.y);
  }

  if (m_windowSizeChanged || m_drawAreaChanged)
  {
    float drawAreaAspectRatio{ m_drawArea.AspectRatio() };
    float windowAspectRatio{ static_cast<float>(m_windowSize.x) / m_windowSize.y };

    Vector2 aspectRatioScale{ std::min(drawAreaAspectRatio / windowAspectRatio, 1.f),
                              std::min(windowAspectRatio / drawAreaAspectRatio, 1.f) };
    Matrix3 t{ Matrix3::Identity() };
    t *= Matrix3::Scale((2.f / m_drawArea.Size()).cwiseProduct(aspectRatioScale));
    t *= Matrix3::Translate({ -aspectRatioScale });
    t *= GetViewportTransform();
    m_program.SetUniformValue(m_program.GetUniformLocation("inputTransform"), t);
  }
  m_windowSizeChanged = false;
  m_drawAreaChanged = false;

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_program.Use();

  m_vao.Bind();
  glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(m_triangles.size() * 3));
  m_vao.Unbind();

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
  glBlitFramebuffer(
    0, 0, m_windowSize.x, m_windowSize.y, 0, 0, m_windowSize.x, m_windowSize.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);

  CheckError();
}

Vector2 Renderer::GetNormalizedMousePosition(const Vector2i& mousePosition)
{
  return Vector2{ m_windowSize.x - mousePosition.x, mousePosition.y }.cwiseQuotient(Vector2{ m_windowSize });
}

Matrix3 Renderer::GetViewportTransform() const
{
  float zoom{ std::pow(ZOOM_BASE, static_cast<float>(m_zoomLevel)) };
  return Matrix3::Translate(m_pan) * Matrix3::Scale(Vector2{ zoom });
}

void Renderer::RecreateFramebuffer()
{
  if (m_fbo != 0)
    glDeleteFramebuffers(1, &m_fbo);

  glGenFramebuffers(1, &m_fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

  if (m_maxSampleCount == -1)
    glGetIntegerv(GL_MAX_SAMPLES, &m_maxSampleCount);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
  glTexImage2DMultisample(
    GL_TEXTURE_2D_MULTISAMPLE, std::min(m_maxSampleCount, 16), GL_RGB, m_windowSize.x, m_windowSize.y, GL_TRUE);
  glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
  GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, drawBuffers);
  if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "Framebuffer error\n";

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glDeleteTextures(1, &texture);

  CheckError();
}

void Renderer::SaveScreenshotAsPNG(const std::filesystem::path& outputPath)
{
  // A framebuffer which writes to a multisample-texture cannot be used for glReadPixels, therefore an additional
  // framebuffer and texture without multisample needs to be used

  GLuint textureFbo;
  glGenFramebuffers(1, &textureFbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, textureFbo);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_windowSize.x, m_windowSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
  GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, drawBuffers);
  if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "Framebuffer error\n";

  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, textureFbo);
  glBlitFramebuffer(
    0, 0, m_windowSize.x, m_windowSize.y, 0, 0, m_windowSize.x, m_windowSize.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);

  std::vector<std::byte> imageData(m_windowSize.x * m_windowSize.y * 3);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, textureFbo);
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glReadPixels(0, 0, m_windowSize.x, m_windowSize.y, GL_RGB, GL_UNSIGNED_BYTE, imageData.data());

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glDeleteFramebuffers(1, &textureFbo);
  glDeleteTextures(1, &texture);
  CheckError();

  // Better not use stbi_flip_vertically_on_write() and change the global state of stb_image, flip image by using a
  // negative stride instead
  int lineSize{ m_windowSize.x * 3 };
  std::string pathString{ outputPath.string() };
  stbi_write_png(pathString.c_str(),
                 m_windowSize.x,
                 m_windowSize.y,
                 3,
                 imageData.data() + lineSize * (m_windowSize.y - 1),
                 -lineSize);
}
}
