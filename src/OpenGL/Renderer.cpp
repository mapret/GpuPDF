#include "Renderer.hpp"
#include "OpenGL/Buffer.hpp"
#include "OpenGL/Error.hpp"
#include "Window.hpp"
#include <format>
#include <GL/glew.h>
#include <iostream>

namespace
{
const char* scalingVertexShader{ R"""(#version 330 core
layout(location = 0) in vec2 position2d;
layout(location = 1) in vec3 color;
out vec3 colorPS;
uniform vec4 inputScaling;
void main() {
  gl_Position = vec4(position2d * inputScaling.xy + inputScaling.zw, 0.f, 1.f);
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
Renderer::Renderer(Window& window)
  : m_program(scalingVertexShader, passthroughFragmentShader)
{
  CheckError();

  glEnable(GL_MULTISAMPLE);
  glDisable(GL_DEPTH_TEST);

  window.SetMouseMoveCallback(
    [this](const Vector2i& position)
  {
    if (m_leftButtonPressed)
    {
      Vector2i movement{ m_lastMousePosition - position };
      m_pan.x += static_cast<float>(movement.x); // TODO: Change once a convert-constructor for Matrix exists
      m_pan.y += static_cast<float>(movement.y);
      m_lastMousePosition = position;
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
        m_lastMousePosition = position;
      }
      else
      {
        m_leftButtonPressed = false;
      }
    }
  });
  window.SetMouseWheelHandler(
    [this](int offset, const Vector2i& mousePosition)
  {
    m_zoomLevel = std::clamp(m_zoomLevel + offset, MIN_ZOOM_LEVEL, MAX_ZOOM_LEVEL);
    m_drawAreaChanged = true;
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
    Vector4 scaling{ 1.f, 1.f, -1.f, -1.f };

    float drawAreaAspectRatio{ m_drawArea.AspectRatio() };
    float windowAspectRatio{ static_cast<float>(m_windowSize.x) / m_windowSize.y };
    float zoom{ std::pow(ZOOM_BASE, static_cast<float>(m_zoomLevel)) };
    if (windowAspectRatio > drawAreaAspectRatio) // height-restricted
    {
      float heightScale{ 1.f / m_drawArea.Height() * 2 };
      scaling.x = heightScale / windowAspectRatio;
      scaling.y = heightScale;
      scaling.z = -drawAreaAspectRatio / windowAspectRatio;
    }
    else // width-restricted
    {
      float widthScale{ 1.f / m_drawArea.Width() * 2 };
      scaling.x = widthScale;
      scaling.y = widthScale * windowAspectRatio;
      scaling.w = -windowAspectRatio / drawAreaAspectRatio;
    }
    scaling.x *= zoom;
    scaling.y *= zoom;
    scaling.z -= m_pan.x / m_windowSize.x * 2;
    scaling.w += m_pan.y / m_windowSize.y * 2;

    m_program.SetUniformValue(m_program.GetUniformLocation("inputScaling"), scaling);
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

void Renderer::RecreateFramebuffer()
{
  if (m_fbo != 0)
    glDeleteFramebuffers(1, &m_fbo);

  glGenFramebuffers(1, &m_fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, GL_RGB, m_windowSize.x, m_windowSize.y, GL_TRUE);
  glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
  GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, drawBuffers);
  if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "Framebuffer error\n";

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glDeleteTextures(1, &texture);

  CheckError();
}
}
