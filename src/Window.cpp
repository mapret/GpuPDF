#include "Window.hpp"
#include "OpenGL/Renderer.hpp"
#include "PDFStreamFinder.hpp"
#include "PDFStreamReader.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>

using namespace MouseEvents;

Window* Window::m_self{ nullptr };

Window::Window()
{
  if (m_self != nullptr)
    std::cerr << "Multiple windows are not supported\n";
  m_self = this;
}

void Window::Run(const std::filesystem::path& sourceFile)
{
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  glfwInit();
  GLFWwindow* window{ glfwCreateWindow(800, 600, "GpuPDF", nullptr, nullptr) };
  glfwMakeContextCurrent(window);

  auto rendererPtr{ std::make_unique<gl::Renderer>(*this) };
  auto& renderer{ *rendererPtr };

  std::thread loadThread{ [&]()
  {
    auto graphicStreams{ PDFStreamFinder{}.GetGraphicsStreams(sourceFile) };
    PDFStreamReader reader;
    for (const auto& stream : graphicStreams)
    {
      reader.Read(stream);
    }
    renderer.AddTriangles(reader.CollectTriangles());
    renderer.SetDrawArea(reader.GetDrawArea());
    renderer.Finish();
  } };

  glfwSetCursorPosCallback(window, &Window::CursorPositionCallback_impl);
  glfwSetMouseButtonCallback(window, &Window::MouseButtonCallback_impl);
  glfwSetScrollCallback(window, &Window::ScrollCallback_impl);

  Vector2i oldWindowSize{ 1, 1 };
  Vector2i windowSize{ 2, 2 };

  while (!glfwWindowShouldClose(window))
  {
    glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
    if (windowSize != oldWindowSize)
    {
      renderer.SetWindowSize(windowSize);
      oldWindowSize = windowSize;
    }

    renderer.Draw();
    glfwPollEvents();
    glfwSwapBuffers(window);
  }
  rendererPtr.reset(); // Do OpenGL cleanup before the window is destroyed
  glfwDestroyWindow(window);
  glfwTerminate();

  loadThread.join();
}

void Window::SetMouseMoveCallback(const MouseMoveCallback& callback)
{
  m_mouseMoveCallback = callback;
}

void Window::SetMouseButtonCallback(const MouseButtonCallback& callback)
{
  m_mouseButtonCallback = callback;
}

void Window::SetMouseWheelHandler(const MouseWheelCallback& callback)
{
  m_mouseWheelCallback = callback;
}

void Window::CursorPositionCallback_impl(GLFWwindow* window, double xPosition, double yPosition)
{
  m_self->m_currentMousePosition = Vector2i{ static_cast<int>(xPosition), static_cast<int>(yPosition) };
  if (m_self->m_mouseMoveCallback)
    m_self->m_mouseMoveCallback(m_self->m_currentMousePosition);
}

void Window::MouseButtonCallback_impl(GLFWwindow* window, int button, int action, int mods)
{
  if (m_self->m_mouseButtonCallback)
  {
    MouseButton mouseButton;
    switch (button)
    {
      case GLFW_MOUSE_BUTTON_LEFT:
        mouseButton = MouseButton::Left;
        break;
      case GLFW_MOUSE_BUTTON_MIDDLE:
        mouseButton = MouseButton::Middle;
        break;
      case GLFW_MOUSE_BUTTON_RIGHT:
        mouseButton = MouseButton::Right;
        break;
    }

    MouseAction mouseAction;
    switch (action)
    {
      case GLFW_PRESS:
        mouseAction = MouseAction::Press;
        break;
      case GLFW_RELEASE:
        mouseAction = MouseAction::Release;
        break;
    }

    m_self->m_mouseButtonCallback(mouseButton, mouseAction, m_self->m_currentMousePosition);
  }
}

void Window::ScrollCallback_impl(GLFWwindow* window, double xOffset, double yOffset)
{
  if (m_self->m_mouseWheelCallback)
    m_self->m_mouseWheelCallback(static_cast<int>(yOffset), m_self->m_currentMousePosition);
}
