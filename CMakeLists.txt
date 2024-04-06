cmake_minimum_required(VERSION 3.20)

if (WIN32)
  set(VCPKG_TARGET_TRIPLET x64-windows-static)
endif ()
#set(VCPKG_MANIFEST_MODE ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

if (NOT CMAKE_TOOLCHAIN_FILE)
  include(FetchContent)
  FetchContent_Declare(
      vcpkg
      GIT_REPOSITORY https://github.com/microsoft/vcpkg/
      GIT_TAG 2024.02.14
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
      USE_TERMINAL_DOWNLOAD TRUE
  )
  FetchContent_MakeAvailable(vcpkg)
  set(CMAKE_TOOLCHAIN_FILE ${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake CACHE FILEPATH "")

  function(checkAppendBuildType triplet_name)
    set(triplet_name ${vcpkg_SOURCE_DIR}/triplets/${triplet_name}.cmake)
    set(add_text "set(VCPKG_BUILD_TYPE release)")
    file(READ ${triplet_name} tripled_src)
    string(FIND "${tripled_src}" "${add_text}" find_result)
    if (${find_result} EQUAL -1)
      file(APPEND ${triplet_name} "${add_text}\n")
    endif()
  endfunction()
  checkAppendBuildType(x64-windows-static)
  checkAppendBuildType(x64-linux)
endif ()
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(gpupdf CXX)

include(cmake/Warnings.cmake)

file(GLOB_RECURSE gpupdf_sources "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.hpp")
add_executable(gpupdf ${gpupdf_sources})

target_compile_definitions(gpupdf PRIVATE GLEW_NO_GLU)
target_include_directories(gpupdf PRIVATE ${CMAKE_SOURCE_DIR}/src)
#target_include_directories(gpupdf PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(CDT CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(ZLIB REQUIRED)
target_link_libraries(gpupdf PRIVATE glfw)
target_link_libraries(gpupdf PRIVATE GLEW::GLEW)
target_link_libraries(gpupdf PRIVATE CDT::CDT)
target_link_libraries(gpupdf PRIVATE ZLIB::ZLIB)
target_include_directories(gpupdf PRIVATE ${Stb_INCLUDE_DIR})

if (LINUX)
  find_package(TBB CONFIG REQUIRED)
  target_link_libraries(gpupdf PRIVATE TBB::tbb)
endif ()
