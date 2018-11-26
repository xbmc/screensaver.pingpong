project(screensaver.pingpong)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(Kodi REQUIRED)

if(NOT WIN32 AND (APP_RENDER_SYSTEM STREQUAL "gl" OR NOT APP_RENDER_SYSTEM))
  find_package(OpenGL REQUIRED)
  set(DEPLIBS ${OPENGL_LIBRARIES})
  set(includes ${OPENGL_INCLUDE_DIR})
  add_definitions(-DHAS_OPENGL)
elseif(NOT WIN32 AND APP_RENDER_SYSTEM STREQUAL "gles")
  find_package(OpenGLES2 REQUIRED)
  set(DEPLIBS ${OPENGLES2_LIBRARIES})
  set(includes ${OPENGLES2_INCLUDE_DIR})
  add_definitions(-DHAS_GLES2)
endif()

if(NOT WIN32)
  set(PINGPONG_SOURCES src/shaders/GUIShader.cpp
                       src/shaders/Matrix.cpp
                       src/shaders/Shader.cpp)
  set(PINGPONG_HEADERS src/shaders/GUIShader.h
                       src/shaders/Matrix.h
                       src/shaders/Shader.h)
endif()

include_directories(${includes} ${KODI_INCLUDE_DIR}/..) # Hack way with "/..", need bigger Kodi cmake rework to match right include ways

list(APPEND PINGPONG_SOURCES src/main.cpp
                             src/pingpong.cpp)

list(APPEND PINGPONG_HEADERS src/main.h
                             src/pingpong.h
                             src/timer.h
                             src/types.h)

build_addon(screensaver.pingpong PINGPONG DEPLIBS)

include(CPack)
