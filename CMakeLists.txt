cmake_minimum_required(VERSION 3.20)

set(VCPKG_TARGET_TRIPLET x64-windows-static)
#set(VCPKG_MANIFEST_MODE ON)

set(CMAKE_CXX_STANDARD 20)

if (NOT CMAKE_TOOLCHAIN_FILE)
  include(FetchContent)
  FetchContent_Declare(
      vcpkg
      GIT_REPOSITORY https://github.com/microsoft/vcpkg/
      GIT_TAG 2023.07.21
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
      USE_TERMINAL_DOWNLOAD TRUE
  )
  FetchContent_MakeAvailable(vcpkg)
  set(CMAKE_TOOLCHAIN_FILE "${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "")
endif ()
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(gpupdf C CXX)

include(cmake/Warnings.cmake)

file(GLOB_RECURSE gpupdf_sources "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.hpp")
add_executable(gpupdf ${gpupdf_sources})

target_include_directories(gpupdf PRIVATE ${CMAKE_SOURCE_DIR}/src)
#target_include_directories(gpupdf PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)

find_package(podofo CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(CDT CONFIG REQUIRED)
find_package(Stb REQUIRED)
target_link_libraries(gpupdf PRIVATE $<IF:$<TARGET_EXISTS:podofo_shared>,podofo_shared,podofo_static>)
target_link_libraries(gpupdf PRIVATE glfw)
target_link_libraries(gpupdf PRIVATE GLEW::GLEW)
target_link_libraries(gpupdf PRIVATE CDT::CDT)
target_include_directories(gpupdf PRIVATE ${Stb_INCLUDE_DIR})
