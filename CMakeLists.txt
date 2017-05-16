project(screensaver.pingpong)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(Kodi REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${KODI_INCLUDE_DIR})

set(PINGPONG_SOURCES src/main.cpp
                     src/pingpong.cpp)

set(PINGPONG_HEADERS src/main.h
                     src/pingpong.h
                     src/timer.h
                     src/types.h)

set(DEPLIBS ${OPENGL_LIBRARIES})

build_addon(screensaver.pingpong PINGPONG DEPLIBS)

include(CPack)
