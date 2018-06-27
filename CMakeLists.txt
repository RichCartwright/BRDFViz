cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME "BRDFViz")
set(EXECUTABLE_NAME ${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

##set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
project(
  ${PROJECT_NAME}
  VERSION 0.1
  LANGUAGES CXX
  )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(FindPackageHandleStandardArgs)

## Assimp
find_package(assimp REQUIRED)

## GLM
set(GLM_FIND_REQUIRED TRUE)
find_path(
  GLM_INCLUDE_DIRS "glm/glm.hpp"
  )
find_package_handle_standard_args(
  GLM
  DEFAULT_MSG
  GLM_INCLUDE_DIRS
  )

## libpng, PNG++
find_package(PNG REQUIRED)
set(png++_FIND_REQUIRED TRUE)
find_path(
  png++_INCLUDE_DIRS
  NAMES "png++/color.hpp" "png++/png.hpp"
  )
find_package_handle_standard_args(
  png++
  DEFAULT_MSG
  png++_INCLUDE_DIRS
  )

## libjpeg
find_package(JPEG REQUIRED)

## OpenEXR
find_package(OpenEXR REQUIRED)

## VTK
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})

add_subdirectory(src)
