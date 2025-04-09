cmake_minimum_required(VERSION 3.10)
project(Particle LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(OpenMP)
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

link_directories(${CMAKE_SOURCE_DIR}/packages/glfw/lib)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/packages)

file(GLOB_RECURSE SOURCE_FILES 
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.c 
    ${CMAKE_SOURCE_DIR}/src/*.h
)
add_executable(${PROJECT_NAME} ${SOURCE_FILES} )

target_link_libraries(${PROJECT_NAME}  glfw3.lib opengl32.lib)
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${SOURCE_FILES})



