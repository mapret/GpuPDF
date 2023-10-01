#pragma once

#include "Vector.hpp"

class Rectangle
{
public:
  Vector2 min;
  Vector2 max;

  float Width() const { return max.x - min.x; }
  float Height() const { return max.y - min.y; }
  float AspectRatio() const { return Width() / Height(); }
};
