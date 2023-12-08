cmake_minimum_required(VERSION 3.23.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_SOURCE_DIR}/cmake/vcpkg_bootstrap.cmake)

#########################
# platform specific settings, windows, linux, freebsd
#########################
if (WIN64)
    set (VCPKG_TARGET_TRIPLET x64-windows)
elseif(UNIX)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set (VCPKG_TARGET_TRIPLET x64-linux)
    elseif (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
        set (VCPKG_TARGET_TRIPLET x64-freebsd)
    endif()
endif()

# vcpkg
set(VCPKG_BOOTSTRAP_OPTIONS "-disableMetrics")
set(VCPKG_INSTALL_OPTIONS "--no-print-usage")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")
list(APPEND VCPKG_FEATURE_FLAGS manifests)
x_vcpkg_bootstrap()

project(HelloWorld LANGUAGES CXX)

find_package(imgui CONFIG REQUIRED)
find_package(stdexec CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)

set (Boost_NO_WARN_NEW_VERSIONS 1)
find_package(Boost REQUIRED COMPONENTS graph)

add_executable(main
  src/main.cpp
  src/application.cpp
  src/application.h
  src/widget.cpp
  src/widget.h
  src/imgui_demo.cpp
)

target_include_directories(main
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(main
  PRIVATE
    Boost::graph
    ${Boost_LIBRARIES}
    uuid
    imgui::imgui
    TBB::tbb
    TBB::tbbmalloc
    STDEXEC::stdexec
)