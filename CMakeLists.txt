cmake_minimum_required(VERSION 3.12)
project(screensaver.pingpong)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)

if(NOT WIN32 AND (APP_RENDER_SYSTEM STREQUAL "gl" OR NOT APP_RENDER_SYSTEM))
  find_package(OpenGl REQUIRED)
  set(DEPLIBS ${OPENGL_LIBRARIES})
  set(includes ${OPENGL_INCLUDE_DIR})
  add_definitions(${OPENGL_DEFINITIONS})
  if(CORE_PLATFORM_NAME STREQUAL "gbm")
    add_definitions(-DSOIL_EGL)
  endif()
else()
  find_package(OpenGLES REQUIRED)
  #set(DEPLIBS ${OPENGLES_LIBRARIES})
  set(DEPLIBS "")#${OPENGLES_LIBRARIES})
  set(includes ${OPENGLES_INCLUDE_DIR})
  add_definitions(${OPENGLES_DEFINITIONS})
endif()

if(WIN32)
  # dlfcn implementation for windows, allow to use e.g. dlopen as POSIX-compatible way
  find_package(dlfcn-win32 REQUIRED)
  list(APPEND DEPLIBS ${dlfcn-win32_LIBRARIES})
  include_directories(${dlfcn-win32_INCLUDE_DIRS})

  get_property(libGLESv2_definitions TARGET kodi::angle::libGLESv2 PROPERTY INTERFACE_COMPILE_DEFINITIONS)
  add_compile_definitions(${libGLESv2_definitions})
  get_property(libGLESv2_location TARGET kodi::angle::libGLESv2 PROPERTY LOCATION)
  list(APPEND PINGPONG_ADDITIONAL_BINARY ${libGLESv2_location})
  get_property(libEGL_location TARGET kodi::angle::libEGL PROPERTY LOCATION)
  list(APPEND PINGPONG_ADDITIONAL_BINARY ${libEGL_location})
 endif()

include_directories(${GLM_INCLUDE_DIR}
                    ${KODI_INCLUDE_DIR}/..) # Hack way with "/..", need bigger Kodi cmake rework to match right include ways

set(PINGPONG_SOURCES src/main.cpp
                     src/pingpong.cpp)

set(PINGPONG_HEADERS src/main.h
                     src/pingpong.h
                     src/timer.h
                     src/types.h)

build_addon(screensaver.pingpong PINGPONG DEPLIBS)

include(CPack)
