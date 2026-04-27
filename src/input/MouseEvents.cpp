module;

#include <functional>

export module gpupdf.input:MouseEvents;

import gpupdf.math;

export namespace MouseEvents
{
enum class MouseButton
{
  Left,
  Middle,
  Right,
};

enum class MouseAction
{
  Press,
  Release,
};

using MouseMoveCallback = std::function<void(const Vector2i& mousePosition)>;
using MouseButtonCallback =
  std::function<void(MouseButton mouseButton, MouseAction mouseAction, const Vector2i& mousePosition)>;
using MouseWheelCallback = std::function<void(int scrollAmountY, const Vector2i& mousePosition)>;
} // namespace MouseEvents
