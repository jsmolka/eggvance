cmake_minimum_required(VERSION 3.8)

project(eggvance)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -flto")

find_package(SDL2 REQUIRED)

add_definitions(-DFMT_HEADER_ONLY)
add_definitions(-DSDL_MAIN_HANDLED)

include_directories(${SDL2_INCLUDE_DIRS})
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

target_link_libraries(${CMAKE_PROJECT_NAME} ${SDL2_LIBRARIES})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(${CMAKE_PROJECT_NAME} stdc++fs)
endif()
