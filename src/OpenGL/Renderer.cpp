#include "Renderer.hpp"
#include "OpenGL/Buffer.hpp"
#include "OpenGL/Error.hpp"
#include <format>
#include <gl/glew.h>
#include <iostream>

namespace
{
const char* scalingVertexShader{ R"""(#version 330 core
layout(location = 0) in vec2 position2d;
uniform vec4 inputScaling;
void main() {
  gl_Position = vec4(position2d * inputScaling.xy + inputScaling.zw, 0.f, 1.f);
}
)""" };

const char* passthroughFragmentShader{ R"""(#version 330 core
layout(location = 0) out vec3 color;
void main() {
  color = vec3(1, 0, 0);
}
)""" };
}

namespace gl
{
Renderer::Renderer()
  : m_program(scalingVertexShader, passthroughFragmentShader)
{
  CheckError();

  glEnable(GL_MULTISAMPLE);
  glDisable(GL_DEPTH_TEST);
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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

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
