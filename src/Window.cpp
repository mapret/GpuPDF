#include "Window.hpp"
#include "OpenGL/Renderer.hpp"
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

  auto rendererPtr{ std::make_unique<gl::Renderer>() };
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
