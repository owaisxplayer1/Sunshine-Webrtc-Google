cmake_minimum_required(VERSION 3.22)

project(
        webrtc
        VERSION 2.4.0
        LANGUAGES C CXX ASM)

cmake_policy(SET CMP0076 NEW)

set(Windows TRUE)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE ON)
# set iterator debug level and compiler flags.
if(Windows AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG
            "${CMAKE_CXX_FLAGS_DEBUG} -D_ITERATOR_DEBUG_LEVEL=0 /Zi /Od")
else()
    set(CMAKE_CXX_FLAGS_DEBUG
            "${CMAKE_CXX_FLAGS_DEBUG} -D_ITERATOR_DEBUG_LEVEL=0")
endif()


set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(WebRTC REQUIRED)

find_package(Vulkan REQUIRED)

find_package(CUDA REQUIRED)

add_subdirectory(WebRTCPlugin)

