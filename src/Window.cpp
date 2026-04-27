module;

#include <GLFW/glfw3.h>
#include <filesystem>
#include <iostream>
#include <thread>

export module gpupdf.window;

import gpupdf.input;
import gpupdf.math;
import gpupdf.renderer.opengl;
import gpupdf.pdf;

export class Window
{
  InputHandler m_inputHandler;
  static Window* m_self;

  static void CursorPositionCallback_impl(GLFWwindow* window, double xPosition, double yPosition);
  static void MouseButtonCallback_impl(GLFWwindow* window, int button, int action, int mods);
  static void ScrollCallback_impl(GLFWwindow* window, double xOffset, double yOffset);

public:
  Window();
  ~Window();
  void Run(const std::filesystem::path& sourceFile);
};

Window* Window::m_self{ nullptr };

Window::Window()
{
  if (m_self != nullptr)
    std::cerr << "Multiple windows are not supported\n";
  m_self = this;
}

Window::~Window()
{
  if (!m_self)
    std::cerr << "Window context not found\n";
  m_self = nullptr;
}

void Window::Run(const std::filesystem::path& sourceFile)
{
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  glfwInit();
  GLFWwindow* window{ glfwCreateWindow(800, 600, "GpuPDF", nullptr, nullptr) };
  glfwMakeContextCurrent(window);

  Vector2 dpi;
  glfwGetWindowContentScale(window, &dpi.x, &dpi.y);
  auto rendererPtr{ std::make_unique<gl::Renderer>(m_inputHandler, dpi) };
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

void Window::CursorPositionCallback_impl(GLFWwindow* /*window*/, double xPosition, double yPosition)
{
  m_self->m_inputHandler.MouseMoveEvent(Vector2i{ static_cast<int>(xPosition), static_cast<int>(yPosition) });
}

void Window::MouseButtonCallback_impl(GLFWwindow* /*window*/, int button, int action, int /*mods*/)
{
  using MouseEvents::MouseAction;
  using MouseEvents::MouseButton;

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
    default:
      return;
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
    default:
      return;
  }

  m_self->m_inputHandler.MouseButtonEvent(mouseButton, mouseAction);
}

void Window::ScrollCallback_impl(GLFWwindow* /*window*/, double /*xOffset*/, double yOffset)
{
  m_self->m_inputHandler.MouseWheelEvent(static_cast<int>(yOffset));
}
