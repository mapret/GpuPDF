#include "Window.hpp"

int main(int argc, char** argv)
{
  if (argc >= 1)
  {
    Window window;
    window.Run(argv[1]);
  }

  return 0;
}
