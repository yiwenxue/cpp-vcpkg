cmake_minimum_required(VERSION 3.20)

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

# detect platform
set (VCPKG_TARGET_TRIPLET x64-windows)

project(HelloWorld 
  LANGUAGES CXX)

find_package(unifex CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

set (Boost_NO_WARN_NEW_VERSIONS 1)
find_package(Boost REQUIRED COMPONENTS graph)

add_executable(HelloWorld src/main.cpp)

target_include_directories(HelloWorld
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(HelloWorld
  PRIVATE
  # boost graph
    Boost::graph
    ${Boost_LIBRARIES}
    uuid
  #libunifex
    unifex::unifex
    imgui::imgui
)
