cmake_minimum_required(VERSION 3.8)

project(main)

set(CMAKE_MODULE_PATH /home/kody/Libs/cmake)
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_VERBOSE_MAKEFILE ON)
set(OpenGL_GL_PREFERENCE GLVND)
set(CMAKE_C_FLAGS -fprofile-arcs )
set(CMAKE_C_FLAGS -ftest-coverage)
set(CMAKE_EXE_LINKER_FLAGS -lgcov )
set(CMAKE_EXE_LINKER_FLAGS --coverage)

add_executable(main  D_Object.cpp Shader.cpp 
	main.cpp 
	convenience.cpp ogl.cpp display.cpp )


include_directories(/usr/local/include/reactphysics3d)
find_package(ReactPhysics3D REQUIRED)
if(${ReactPhysics3D_FOUND})
	MESSAGE(STATUS "Found RP3D at ${ReactPhysics3D_DIR}")
ELSE(${ReactPhysics3D_FOUND})
	MESSAGE(STATUS "Did not find RP3D")
ENDIF(${ReactPhysics3D_FOUND})
	
target_link_libraries(main 
	ReactPhysics3D::ReactPhysics3D)


include_directories(Libs/SOIL2/incs)
add_library(SOIL2 STATIC IMPORTED)
set_target_properties(SOIL2 PROPERTIES IMPORTED_LOCATION Libs/SOIL2/libSOIL2.a)
target_link_libraries(main SOIL2)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
target_link_libraries(main ${GLFW_LIBRARIES})

find_package(GLM REQUIRED)
include_directories({${GLM_INCLUDE_DIRS})
target_link_libraries(main ${GLM_LIBRARIES})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(main ${GLEW_LIBRARIES})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

target_link_libraries(main OpenGL::GL)

