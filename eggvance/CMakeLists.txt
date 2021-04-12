cmake_minimum_required(VERSION 3.8)

project(eggvance)

if (UNIX AND NOT APPLE AND NOT ANDROID)
  set(LINUX TRUE)
endif()

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
  ${PROJECT_SOURCE_DIR}/modules/nfd/*.h
  ${PROJECT_SOURCE_DIR}/modules/nfd/*.c
  ${PROJECT_SOURCE_DIR}/src/*.h
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

if (APPLE)
  list(APPEND SOURCE_FILES ${PROJECT_SOURCE_DIR}/modules/nfd/nfd_cocoa.m)
elseif (LINUX)
  list(APPEND SOURCE_FILES ${PROJECT_SOURCE_DIR}/modules/nfd/nfd_gtk.c)
endif()

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${CMAKE_PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} OpenGL::GL ${CMAKE_DL_LIBS})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(${CMAKE_PROJECT_NAME} stdc++fs)
endif()

if (LINUX)
  find_package(PkgConfig)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(GTK "gtk+-3.0")
    if (GTK_FOUND)
      target_link_libraries(${CMAKE_PROJECT_NAME} ${GTK_LIBRARIES})
      add_definitions(${GTK_CFLAGS} ${GTK_CFLAGS_OTHER})
    endif()
  endif()
endif()
