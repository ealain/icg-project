# the exercise name is nothing else than the directory
get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h" ".hpp")

message(STATUS "Copying textures to build folder")
file(COPY textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


# list all the files you want to copy everytime
# you build (ie, you want the shaders there)
file(GLOB SHADERS
  grid/*.glsl
  noise/*.glsl
  sky/*.glsl
  water/*.glsl
  shadow/*.glsl)
deploy_shaders_to_build_dir(${SHADERS})

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS})
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
