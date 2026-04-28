if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(
      /W4
      /permissive
      /experimental:module
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
  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "20.1.0")
    add_compile_options(-Wno-import-implementation-partition-unit-in-interface-unit)
  endif()
else ()
  message(AUTHOR_WARNING "Unrecognized compiler \"${CMAKE_CXX_COMPILER_ID}\", consider adding it in Warnings.cmake")
endif ()
