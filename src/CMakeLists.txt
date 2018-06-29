link_directories(
  ${png++_LIBRARY_DIRS}
  ${assimp_LIBRARY_DIRS}
  ${JPEG_LIBRARY_DIRS}
  ${OPENEXR_LIBRARY_DIRS}
  ${VTK_LIBRARY_DIRS}
  )

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${GLM_INCLUDE_DIRS}
  ${png++_INCLUDE_DIRS}
  ${assimp_INCLUDE_DIRS}
  ${JPEG_INCLUDE_DIRS}
  ${OPENEXR_INCLUDE_DIRS}
  SYSTEM ../external/
  )

add_definitions(
  -std=c++11
  -Wall
  #-Wextra
  -Werror
  # Debug config:
  # -g
  # Test config:
  -O3
  # Release config:
  #  -O3 -DNDEBUG -Wno-unused
  )

file(GLOB SOURCES
  ./*.cpp
  ./LTC/*.cpp
  ./bxdf/*.cpp
  ../external/*.cpp
  )

file(GLOB UI_FILES
  *.ui
  )

file(GLOB QT_WRAP 
  ./*.h
  )

if(${VTK_VERSION} VERSION_GREATER "6" AND VTK_QT_VERSION VERSION_GREATER "4")
  message(STATUS "Wrapping the UI files")
  qt5_wrap_cpp(CPPSrcs ${QT_WRAP})
  qt5_wrap_ui(UISrcs ${UI_FILES})
else()
  message(FATAL "VTK 7 (or higher) and QT 5 (or higher) is needed")
endif()

add_executable(
  ${EXECUTABLE_NAME}
  ${SOURCES}
  ${CPPSrcs}
  ${UISrcs}
  )

target_link_libraries(
  ${EXECUTABLE_NAME}
  ${assimp_LIBRARIES}
  ${PNG_LIBRARY}
  ${JPEG_LIBRARY}
  ${OPENEXR_LIBRARIES}
  ${VTK_LIBRARIES}
  pthread
  )

if(${VTK_VERSION} VERSION_GREATER "6" AND VTK_QT_VERSION VERSION_GREATER "4")
  message(STATUS "Linking QT")
  target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui)
else()
  message(FATAL_ERROR "BRDFViz requires VTK 7 (or higher) and QT 5 (or higher)")
endif()

add_custom_command(
	TARGET ${EXECUTABLE_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${EXECUTABLE_NAME}${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_BINARY_DIR}/"
    )
