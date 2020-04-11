cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto")

set(PLATFORM "SDL" CACHE STRING "Platform <SDL|EMSCRIPTEN>")

if(PLATFORM STREQUAL "SDL")
  project(eggvance)

  add_definitions(-DPLATFORM_SDL)

  find_package(SDL2 REQUIRED)
  include_directories(${SDL2_INCLUDE_DIRS})
elseif(PLATFORM STREQUAL "EMSCRIPTEN")
  project(eggvance.html)

  add_definitions(-DPLATFORM_EMSCRIPTEN)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1 -s FORCE_FILESYSTEM=1 --bind --shell-file ../eggvance.html")
else()
  message(FATAL_ERROR "Invalid platform")
endif()

add_definitions(-DFMT_HEADER_ONLY)

include_directories(external/fmt/include)
include_directories(external/tinytoml/include)
include_directories(icon/cpp)
include_directories(src)

file(GLOB_RECURSE SOURCE_FILES
  ${PROJECT_SOURCE_DIR}/icon/cpp/*.h
  ${PROJECT_SOURCE_DIR}/icon/cpp/*.cpp
  ${PROJECT_SOURCE_DIR}/src/*.h
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

if (PLATFORM STREQUAL "SDL")
  target_link_libraries(${CMAKE_PROJECT_NAME} ${SDL2_LIBRARIES})
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(${CMAKE_PROJECT_NAME} stdc++fs)
endif()
