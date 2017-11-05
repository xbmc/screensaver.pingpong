project(screensaver.pingpong)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(Kodi REQUIRED)
if(NOT CORE_SYSTEM_NAME MATCHES windows)
  find_package(OpenGL REQUIRED)
endif()

include_directories(${KODI_INCLUDE_DIR}/..) # Hack way with "/..", need bigger Kodi cmake rework to match right include ways

set(PINGPONG_SOURCES src/main.cpp
                     src/pingpong.cpp)

set(PINGPONG_HEADERS src/main.h
                     src/pingpong.h
                     src/timer.h
                     src/types.h)

if(NOT CORE_SYSTEM_NAME MATCHES windows)
  set(DEPLIBS ${OPENGL_LIBRARIES})
endif()

build_addon(screensaver.pingpong PINGPONG DEPLIBS)

include(CPack)
