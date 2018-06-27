link_directories(
  ${png++_LIBRARY_DIRS}
  ${assimp_LIBRARY_DIRS}
  ${JPEG_LIBRARY_DIRS}
  ${OPENEXR_LIBRARY_DIRS}
  )

include_directories(
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
  ./NeuralNet/*.cpp
  ../external/*.cpp
  )

add_executable(
  ${EXECUTABLE_NAME}
  ${SOURCES}
  )

## SET_TARGET_PROPERTIES(RGK PROPERTIES LINK_FLAGS -pg)

target_link_libraries(
  ${EXECUTABLE_NAME}
  ${assimp_LIBRARIES}
  ${PNG_LIBRARY}
  ${JPEG_LIBRARY}
  ${OPENEXR_LIBRARIES}
  pthread
  )

add_custom_command(
	TARGET ${EXECUTABLE_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${EXECUTABLE_NAME}${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_BINARY_DIR}/"
    )
