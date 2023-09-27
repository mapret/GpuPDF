#pragma once

#include "MouseEvents.hpp"
#include "math/Vector.hpp"
#include <filesystem>

class GLFWwindow;

class Window
{
  static Window* m_self;

  static void CursorPositionCallback_impl(GLFWwindow* window, double xPosition, double yPosition);
  static void MouseButtonCallback_impl(GLFWwindow* window, int button, int action, int mods);
  static void ScrollCallback_impl(GLFWwindow* window, double xOffset, double yOffset);

  Vector2i m_currentMousePosition;
  MouseEvents::MouseWheelCallback m_mouseWheelCallback;
  MouseEvents::MouseButtonCallback m_mouseButtonCallback;
  MouseEvents::MouseMoveCallback m_mouseMoveCallback;

public:
  Window();
  void Run(const std::filesystem::path& sourceFile);

  void SetMouseMoveCallback(const MouseEvents::MouseMoveCallback& callback);
  void SetMouseButtonCallback(const MouseEvents::MouseButtonCallback& callback);
  void SetMouseWheelHandler(const MouseEvents::MouseWheelCallback& callback);
};
