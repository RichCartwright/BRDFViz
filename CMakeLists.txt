cmake_minimum_required(VERSION 3.0)

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

set(PROJECT_NAME "BRDFViz")
set(EXECUTABLE_NAME ${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

project(
  ${PROJECT_NAME}
  VERSION 1.0
  LANGUAGES CXX
  )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(FindPackageHandleStandardArgs)

## Assimp
find_package(assimp REQUIRED)

# VTK
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

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

if(${VTK_VERSION} VERSION_GREATER "6" AND VTK_QT_VERSION VERSION_GREATER "4")
    set(CMAKE_AUTOMOC ON)
    find_package(Qt5Widgets REQUIRED)
else()
    message([FATAL_ERROR] "BRDFViz requires VTK 7 and QT 5 or higher. \n Exiting processing and generation") 
endif()

add_subdirectory(src)
