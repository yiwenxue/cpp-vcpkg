cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMILER clang++)

include(${CMAKE_SOURCE_DIR}/cmake/vcpkg_bootstrap.cmake)
x_vcpkg_bootstrap()
set(VCPKG_BOOTSTRAP_OPTIONS "-disableMetrics")
set(VCPKG_INSTALL_OPTIONS "--no-print-usage")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")
list(APPEND VCPKG_FEATURE_FLAGS manifests)
set (VCPKG_TARGET_TRIPLET x64-linux)


project(HelloWorld LANGUAGES CXX)

find_package(unifex CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS graph)

add_executable(HelloWorld src/main.cpp)

target_link_libraries(HelloWorld
  PRIVATE
  # boost graph
    Boost::graph
  #libunifex
    unifex::unifex
    imgui::imgui
)
