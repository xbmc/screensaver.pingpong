project(screensaver.pingpong)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(kodi REQUIRED)

include_directories(${KODI_INCLUDE_DIR})

set(PINGPONG_SOURCES src/main.cpp
                     src/pingpong.cpp)

set(DEPLIBS)

build_addon(screensaver.pingpong PINGPONG DEPLIBS)

include(CPack)
