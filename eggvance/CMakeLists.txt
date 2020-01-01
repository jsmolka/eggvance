cmake_minimum_required(VERSION 3.8)

project(eggvance)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -flto")

add_definitions(-DFMT_HEADER_ONLY)

include_directories(external/fmt/include)
include_directories(external/tinytoml/include)
include_directories(icon)
include_directories(src)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

file(GLOB SOURCE_FILES
  ${PROJECT_SOURCE_DIR}/eggvance.rc
  ${PROJECT_SOURCE_DIR}/icon/*.h
  ${PROJECT_SOURCE_DIR}/icon/*.cpp
  ${PROJECT_SOURCE_DIR}/src/*.h
  ${PROJECT_SOURCE_DIR}/src/*.cpp
  ${PROJECT_SOURCE_DIR}/src/arm/*.h
  ${PROJECT_SOURCE_DIR}/src/arm/*.cpp
  ${PROJECT_SOURCE_DIR}/src/common/*.h
  ${PROJECT_SOURCE_DIR}/src/common/*.cpp
  ${PROJECT_SOURCE_DIR}/src/devices/*.h
  ${PROJECT_SOURCE_DIR}/src/devices/*.cpp
  ${PROJECT_SOURCE_DIR}/src/mmu/*.h
  ${PROJECT_SOURCE_DIR}/src/mmu/*.cpp
  ${PROJECT_SOURCE_DIR}/src/mmu/gamepak/*.h
  ${PROJECT_SOURCE_DIR}/src/mmu/gamepak/*.cpp
  ${PROJECT_SOURCE_DIR}/src/platform/sdl/*.h
  ${PROJECT_SOURCE_DIR}/src/platform/sdl/*.cpp
  ${PROJECT_SOURCE_DIR}/src/ppu/*.h
  ${PROJECT_SOURCE_DIR}/src/ppu/*.cpp
  ${PROJECT_SOURCE_DIR}/src/registers/*.h
  ${PROJECT_SOURCE_DIR}/src/registers/*.cpp
  ${PROJECT_SOURCE_DIR}/src/system/*.h
  ${PROJECT_SOURCE_DIR}/src/system/*.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${CMAKE_PROJECT_NAME} ${SDL2_LIBRARIES} stdc++fs)
