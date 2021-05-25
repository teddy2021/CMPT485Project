#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unordered_map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


#ifndef DISPLAY
#define DISPLAY
#include "display.hpp"
#endif

#ifndef CON
#define CON
#include "convenience.hpp"
#endif

#ifndef OGLHPP
#define OGLHPP
#include "ogl.hpp"
#endif

#ifndef DOBJ
#define DOBJ
#include "D_Object.hpp"
#endif

#ifndef SHADE
#define SHADE
#include "Shader.hpp"
#endif

using namespace std;
using namespace glm;

#ifndef DISP
#define DISP

bool Display::context = false;

Display::Display(string title){
	Setup();
	v_matrix = mat4(1);
	p_matrix = mat4(1);

	window = glfwCreateWindow(640, 480, title.c_str(), NULL, NULL);
	if(NULL == window){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU,"
				" they are not 3.3 compatible.\n" );
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if(glewInit() != GLEW_OK){
		fprintf(stderr, "Failed to init GLEW\n");
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glClearColor(0, 0, 0.4f, 0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	

	glEnable(GL_CULL_FACE);

}

Display::Display(string title, float width, float height){

	Setup();
	v_matrix = mat4(1);
	p_matrix = mat4(1);

	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if(NULL == window){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU,"
				" they are not 3.3 compatible.\n" );
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if(glewInit() != GLEW_OK){
		fprintf(stderr, "Failed to init GLEW\n");
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glClearColor(0, 0, 0.4f, 0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	

	glEnable(GL_CULL_FACE);

}

void Display::Setup(){

	if(context == false){
		if(!glfwInit()){
			fprintf( stderr, "Failed to initialize GLFW\n" );
		}

		// Set values for opengl
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		context = true;
	}
}

GLFWwindow * Display::GetWindow(){
	return window;
}

mat4 Display::GetView(){
	return v_matrix;
}

mat4 Display::GetProjection(){
	return p_matrix;
}

#endif

