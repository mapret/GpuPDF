#include "GLRenderer.hpp"
#include <format>
#include <gl/glew.h>
#include <iostream>

namespace
{
void CheckError()
{
  if (auto error{ glGetError() }; error != GL_NO_ERROR)
  {
    std::cerr << "OpenGL Error: " << error << std::format(" (0x{:x})", error) << "\n";
  }
}
}

GLRenderer::GLRenderer()
{
  if (glewInit() != GLEW_OK)
  {
    std::cerr << "Failed to initialize GLEW\n";
  }

  const char* VS{ R"""(#version 330 core
layout(location = 0) in vec2 position2d;
uniform vec4 inputScaling;
void main() {
  //gl_Position = vec4(position2d, 0.f, 1.f);
  gl_Position = vec4(position2d * inputScaling.xy + inputScaling.zw, 0.f, 1.f);
}
)""" };

  const char* FS{ R"""(#version 330 core
out vec3 color;
void main() {
  color = vec3(1, 0, 0);
}
)""" };

  int infoLogLength;

  GLuint vs{ glCreateShader(GL_VERTEX_SHADER) };
  glShaderSource(vs, 1, &VS, nullptr);
  glCompileShader(vs);
  glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 0)
  {
    std::string infoLog(infoLogLength + 1, '\0');
    glGetShaderInfoLog(vs, infoLogLength, nullptr, infoLog.data());
    std::cerr << "VS compile error: " << infoLog;
  }

  GLuint fs{ glCreateShader(GL_FRAGMENT_SHADER) };
  glShaderSource(fs, 1, &FS, nullptr);
  glCompileShader(fs);
  glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 0)
  {
    std::string infoLog(infoLogLength + 1, '\0');
    glGetShaderInfoLog(fs, infoLogLength, nullptr, infoLog.data());
    std::cerr << "FS compile error: " << infoLog;
  }

  m_program = glCreateProgram();
  glAttachShader(m_program, vs);
  glAttachShader(m_program, fs);
  glLinkProgram(m_program);
  glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 0)
  {
    std::string infoLog(infoLogLength + 1, '\0');
    glGetProgramInfoLog(fs, infoLogLength, nullptr, infoLog.data());
    std::cerr << "Shader link error: " << infoLog;
  }

  glDeleteShader(vs);
  glDeleteShader(fs);

  glCreateVertexArrays(1, &m_vao);

  CheckError();
}

GLRenderer::~GLRenderer()
{
  glDeleteProgram(m_program);
  glDeleteVertexArrays(1, &m_vao);
}

void GLRenderer::SetTriangleBuffer(std::vector<Triangle>&& triangles)
{
  m_triangles = triangles;
  m_ready = true;
}

void GLRenderer::SetWindowSize(const Vector2i& windowSize)
{
  m_windowSize = windowSize;
  m_windowSizeChanged = true;
}

void GLRenderer::SetDrawArea(const Rectangle& drawArea)
{
  m_drawArea = drawArea;
  m_drawAreaChanged = true;
}

void GLRenderer::Draw()
{
  if (!m_ready)
    return;

  if (m_initialDraw)
  {
    m_initialDraw = false;

    glBindVertexArray(m_vao);

    unsigned vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, m_triangles.size() * sizeof(m_triangles[0]), m_triangles.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);

    glClearColor(0.9f, 0.9f, 0.9f, 1.f);
    CheckError();
  }

  if (m_windowSizeChanged || m_drawAreaChanged)
  {
    m_windowSizeChanged = false;
    glViewport(0, 0, m_windowSize.x, m_windowSize.y);

    m_drawAreaChanged = false;
    Vector4 scaling{ 1.f, 1.f, 0.f, 0.f };

    float drawAreaAspectRatio{ m_drawArea.AspectRatio() };
    float windowAspectRatio{ static_cast<float>(m_windowSize.x) / m_windowSize.y };
    if (windowAspectRatio > drawAreaAspectRatio) // height-restricted
    {
      float heightScale{ 1.f / m_drawArea.Height() };
      scaling.x = heightScale / windowAspectRatio;
      scaling.y = heightScale;
    }
    else // width-restricted
    {
      float widthScale{ 1.f / m_drawArea.Width() };
      scaling.x = widthScale;
      scaling.y = widthScale * windowAspectRatio;
    }

    glProgramUniform4fv(m_program, glGetUniformLocation(m_program, "inputScaling"), 1, scaling.Data());
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(m_program);

  glBindVertexArray(m_vao);
  glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(m_triangles.size() * 3));
  glBindVertexArray(0);
  CheckError();
}
