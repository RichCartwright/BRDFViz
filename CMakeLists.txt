cmake_minimum_required(VERSION 3.0)

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

set(PROJECT_NAME "BRDFViz")
set(EXECUTABLE_NAME ${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

##set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
project(
  ${PROJECT_NAME}
  VERSION 1.0
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

add_subdirectory(src)

## VTK & QT
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})

if(${VTK_VERSION} VERSION_GREATER "6" AND VTK_QT_VERSION VERSION_GREATER "4")
  # Instruct CMake to run moc automatically when needed.
  set(CMAKE_AUTOMOC ON)
  find_package(Qt5Widgets REQUIRED QUIET)
else()
  find_package(Qt4 REQUIRED)
  include(${QT_USE_FILE})
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB UI_FILES *.ui)
file(GLOB QT_WRAP *.h)
file(GLOB CXX_FILES *.cxx)

if(${VTK_VERSION} VERSION_GREATER "6" AND VTK_QT_VERSION VERSION_GREATER "4")
  qt5_wrap_ui(UISrcs ${UI_FILES} )
  # CMAKE_AUTOMOC in ON so the MOC headers will be automatically wrapped.
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE
    ${CXX_FILES} ${UISrcs} ${QT_WRAP})
    qt5_use_modules(${PROJECT_NAME} Core Gui)
    target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})
else()
  QT4_WRAP_UI(UISrcs ${UI_FILES})
  QT4_WRAP_CPP(MOCSrcs ${QT_WRAP})
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${CXX_FILES} ${UISrcs} ${MOCSrcs})
  target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})
endif()
