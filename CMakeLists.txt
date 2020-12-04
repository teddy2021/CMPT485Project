cmake_minimum_required(VERSION 3.10)

project(physics)

set(CMAKE_MODULE_PATH /home/kody/Libs/cmake)
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(ReactPhysics3D REQUIRED)

include_directories(Libs/SOIL2/incs)
add_library(SOIL2 STATIC IMPORTED)
set_target_properties(SOIL2 PROPERTIES IMPORTED_LOCATION Libs/SOIL2/libSOIL2.a)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})


find_package(GLM REQUIRED glm)
include_directories({${GLM_INCLUDE_DIRS})

find_package(GLEW REQUIRED glew)
include_directories(${GLEW_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

add_executable(main test.cpp convenience.cpp)

target_link_libraries(main 
	ReactPhysics3D::ReactPhysics3D
	${GLFW_LIBRARIES}
	${GLM_LIBRARIES}
	${GLEW_LIBRARIES}
	OpenGL::GL
	SOIL2)

