#pragma once

#include "Vector.hpp"

struct Triangle
{
  struct Vertex
  {
    Vector2 position;
    Vector3 color;
  };

  Vertex a;
  Vertex b;
  Vertex c;

  Triangle(const Vector2& positionA, const Vector2& positionB, const Vector2& positionC, const Vector3& color)
    : a{ positionA, color }
    , b{ positionB, color }
    , c{ positionC, color }
  {
  }
};
