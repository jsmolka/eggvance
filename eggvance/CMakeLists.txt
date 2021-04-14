cmake_minimum_required(VERSION 3.8)

project(eggvance)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto")

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include_directories(modules)
include_directories(modules/shell/include)
include_directories(modules/icon/eggvance/include)
include_directories(src)

file(GLOB_RECURSE SOURCE_FILES
  ${PROJECT_SOURCE_DIR}/modules/glad/*.h
  ${PROJECT_SOURCE_DIR}/modules/glad/*.c
  ${PROJECT_SOURCE_DIR}/modules/imgui/*.h
  ${PROJECT_SOURCE_DIR}/modules/imgui/*.cpp
  ${PROJECT_SOURCE_DIR}/src/*.h
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

list(APPEND SOURCE_FILES ${PROJECT_SOURCE_DIR}/modules/nfd/nfd.h)
list(APPEND SOURCE_FILES ${PROJECT_SOURCE_DIR}/modules/nfd/nfd_common.h)
list(APPEND SOURCE_FILES ${PROJECT_SOURCE_DIR}/modules/nfd/nfd_common.c)

if (APPLE)
  list(APPEND SOURCE_FILES ${PROJECT_SOURCE_DIR}/modules/nfd/nfd_cocoa.m)
elseif (UNIX)
  list(APPEND SOURCE_FILES ${PROJECT_SOURCE_DIR}/modules/nfd/nfd_gtk.c)
endif()

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${CMAKE_PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} OpenGL::GL ${CMAKE_DL_LIBS})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(${CMAKE_PROJECT_NAME} stdc++fs)
endif()

if (APPLE)
  find_library(COCOA_LIBRARY Cocoa)
  target_link_libraries(${CMAKE_PROJECT_NAME} ${COCOA_LIBRARY})
elseif (UNIX)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GTK "gtk+-3.0")

  include_directories(${GTK_INCLUDE_DIRS})
  add_definitions(${GTK_CFLAGS} ${GTK_CFLAGS_OTHER})
  target_link_libraries(${CMAKE_PROJECT_NAME} ${GTK_LIBRARIES})
endif()
