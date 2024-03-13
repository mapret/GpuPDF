#pragma once

#include "OpenGL/GlewInitializer.hpp"
#include "OpenGL/Program.hpp"
#include "OpenGL/VertexArray.hpp"
#include "math/Rectangle.hpp"
#include "math/Triangle.hpp"
#include <filesystem>
#include <vector>

class Window;

namespace gl
{
class Renderer
{
  bool m_ready{ false };
  bool m_initialDraw{ true };

  Vector2 m_dpi;
  Vector2i m_windowSize{ 1, 1 };
  bool m_windowSizeChanged{ true };

  Rectangle m_drawArea{ { 0.f, 0.f }, { 100.f, 100.f } };
  bool m_drawAreaChanged{ true };

  int m_zoomLevel{ 0 };
  Vector2 m_pan{ 0.f };
  constexpr static float ZOOM_BASE{ 1.2f };
  constexpr static int MIN_ZOOM_LEVEL{ -8 };
  constexpr static int MAX_ZOOM_LEVEL{ 16 };

  bool m_leftButtonPressed{ false };
  Vector2 m_lastMousePosition;

  std::vector<Triangle> m_triangles;

  unsigned m_fbo{ 0 };
  int m_maxSampleCount{ -1 };
  GlewInitializer m_glewInitializer;
  VertexArray m_vao;
  Program m_program;

  Vector2 GetNormalizedMousePosition(const Vector2i& mousePosition);
  Matrix3 GetViewportTransform() const;
  void RecreateFramebuffer();

public:
  Renderer(Window& window, const Vector2& dpi);
  ~Renderer();
  void SetTriangleBuffer(std::vector<Triangle>&& triangles);
  void AddTriangles(const std::vector<Triangle>& triangles);
  void Finish();
  void SetWindowSize(const Vector2i& windowSize);
  void SetDrawArea(const Rectangle& drawArea);
  void Draw();

  void SaveScreenshotAsPNG(const std::filesystem::path& outputPath);
};
} // namespace gl
