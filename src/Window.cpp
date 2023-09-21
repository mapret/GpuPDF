#include "Window.hpp"
#include "GLRenderer.hpp"
#include "PDFStreamFinder.hpp"
#include "PDFStreamReader.hpp"
#include <GLFW/glfw3.h>
#include <thread>

void Window::Run(const std::filesystem::path& sourceFile)
{
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  glfwInit();
  GLFWwindow* window{ glfwCreateWindow(800, 600, "GpuPDF", nullptr, nullptr) };
  glfwMakeContextCurrent(window);

  GLRenderer renderer;

  std::thread loadThread{ [&]()
  {
    auto graphicStreams{ PDFStreamFinder{}.GetGraphicsStreams(sourceFile) };
    for (const auto& stream : graphicStreams)
    {
      PDFStreamReader reader{ stream };
      reader.Read();
      auto triangles{ reader.CollectTriangles() };
      renderer.SetTriangleBuffer(std::move(triangles));
      renderer.SetDrawArea(reader.GetDrawArea());
      Vector2i windowSize;
      glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
      renderer.SetWindowSize(windowSize);
    }
  } };

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
  glfwDestroyWindow(window);
  glfwTerminate();

  loadThread.join();
}
