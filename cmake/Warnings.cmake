if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(
      /W4
      /permissive
  )
else ()
  add_compile_options(
      -Wall
      -Wextra
      -Wshadow-all
      -pedantic
  )
endif ()
