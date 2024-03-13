#pragma once

#include "math/Vector.hpp"
#include <functional>

namespace MouseEvents
{
enum class MouseButton
{
  Left,
  Middle,
  Right
};
enum class MouseAction
{
  Press,
  Release
};
using MouseMoveCallback = std::function<void(const Vector2i& mousePosition)>;
using MouseButtonCallback = std::function<void(MouseButton button, MouseAction action, const Vector2i& mousePosition)>;
using MouseWheelCallback = std::function<void(int scrollAmount, const Vector2i& mousePosition)>;
} // namespace MouseEvents
