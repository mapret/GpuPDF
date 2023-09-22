#include "Program.hpp"
#include "Error.hpp"
#include <GL/glew.h>
#include <iostream>
#include <string>

namespace gl
{
Program::Program(const char* vertexShader, const char* fragmentShader)
{
  int infoLogLength;

  GLuint vs{ glCreateShader(GL_VERTEX_SHADER) };
  glShaderSource(vs, 1, &vertexShader, nullptr);
  glCompileShader(vs);
  glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 0)
  {
    std::string infoLog(infoLogLength + 1, '\0');
    glGetShaderInfoLog(vs, infoLogLength, nullptr, infoLog.data());
    std::cerr << "Vertex shader compile error: " << infoLog;
  }

  GLuint fs{ glCreateShader(GL_FRAGMENT_SHADER) };
  glShaderSource(fs, 1, &fragmentShader, nullptr);
  glCompileShader(fs);
  glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 0)
  {
    std::string infoLog(infoLogLength + 1, '\0');
    glGetShaderInfoLog(fs, infoLogLength, nullptr, infoLog.data());
    std::cerr << "Fragment shader compile error: " << infoLog;
  }

  m_name = glCreateProgram();
  glAttachShader(m_name, vs);
  glAttachShader(m_name, fs);
  glLinkProgram(m_name);
  glGetProgramiv(m_name, GL_INFO_LOG_LENGTH, &infoLogLength);
  if (infoLogLength > 0)
  {
    std::string infoLog(infoLogLength + 1, '\0');
    glGetProgramInfoLog(fs, infoLogLength, nullptr, infoLog.data());
    std::cerr << "Shader link error: " << infoLog;
  }

  glDeleteShader(vs);
  glDeleteShader(fs);

  CheckError();
}

Program::~Program()
{
  glDeleteProgram(m_name);

  CheckError();
}

void Program::Use() const
{
  glUseProgram(m_name);
}

int Program::GetUniformLocation(const char* name) const
{
  return glGetUniformLocation(m_name, name);
}
}

void gl::Program::SetUniformValue(int location, const Vector4& vector)
{
  glProgramUniform4fv(m_name, location, 1, vector.Data());
}
