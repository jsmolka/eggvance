cmake_minimum_required(VERSION 3.8)

project(eggvance)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto")

if (NOT EMSCRIPTEN)
  find_package(SDL2 REQUIRED)
  include_directories(${SDL2_INCLUDE_DIRS})
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1 -s FORCE_FILESYSTEM=1 -s DISABLE_EXCEPTION_CATCHING=0 -DEMSCRIPTEN_HAS_UNBOUND_TYPE_NAMES=0 --bind")
endif()

include_directories(external/eggcpt/include)
include_directories(external/icon/themes/default/include)
include_directories(src)

file(GLOB_RECURSE SOURCE_FILES
  ${PROJECT_SOURCE_DIR}/src/*.h
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

if (NOT EMSCRIPTEN)
  target_link_libraries(${CMAKE_PROJECT_NAME} ${SDL2_LIBRARIES})
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(${CMAKE_PROJECT_NAME} stdc++fs)
endif()
