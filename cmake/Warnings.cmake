if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(
      /W4
      /permissive
  )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(
      -Wall
      -Wextra
      -Wshadow
      -pedantic
  )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options(
      -Wall
      -Wextra
      -Wshadow-all
      -pedantic
  )
else ()
  message(AUTHOR_WARNING "Unrecognized compiler \"${CMAKE_CXX_COMPILER_ID}\", consider adding it in Warnings.cmake")
endif ()
