module;

export module gpupdf.input:InputHandler;

import :MouseEvents;
import gpupdf.math;

export class InputHandler
{
  Vector2i m_currentMousePosition;
  MouseEvents::MouseWheelCallback m_mouseWheelCallback;
  MouseEvents::MouseButtonCallback m_mouseButtonCallback;
  MouseEvents::MouseMoveCallback m_mouseMoveCallback;

public:
  // Explicitly defined constructor and destructor required by MinGW
  InputHandler() {}
  ~InputHandler() {}

  void MouseMoveEvent(const Vector2i& mousePosition)
  {
    m_currentMousePosition = mousePosition;
    if (m_mouseMoveCallback)
      m_mouseMoveCallback(mousePosition);
  }

  void MouseButtonEvent(MouseEvents::MouseButton button, MouseEvents::MouseAction action)
  {
    if (m_mouseButtonCallback)
      m_mouseButtonCallback(button, action, m_currentMousePosition);
  }

  void MouseWheelEvent(int yOffset)
  {
    if (m_mouseWheelCallback)
      m_mouseWheelCallback(yOffset, m_currentMousePosition);
  }

  void SetMouseMoveCallback(const MouseEvents::MouseMoveCallback& callback)
  {
    m_mouseMoveCallback = callback;
  }

  void SetMouseButtonCallback(const MouseEvents::MouseButtonCallback& callback)
  {
    m_mouseButtonCallback = callback;
  }

  void SetMouseWheelHandler(const MouseEvents::MouseWheelCallback& callback)
  {
    m_mouseWheelCallback = callback;
  }
};
