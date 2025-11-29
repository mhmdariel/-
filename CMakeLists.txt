cmake_minimum_required(VERSION 3.12)
project(UniversalAudioGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -ffast-math")
endif()

add_executable(UniversalAudioGenerator main.cpp)

# For future audio library integration
find_package(PkgConfig)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
