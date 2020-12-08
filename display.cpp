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

extern float width, height;

unordered_map<string, 

GLFWwindow * setup(){

	if(!glfwInit()){
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return (GLFWwindow *)-1;
	}

	// Set values for opengl
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create new window
	GLFWwindow * window;
	window = glfwCreateWindow(width, height, "test", NULL, NULL);
	if(NULL == window){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return (GLFWwindow *)-1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if(glewInit() != GLEW_OK){
		fprintf(stderr, "Failed to init GLEW\n");
		return (GLFWwindow *)-1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glClearColor(0, 0, 0.4f, 0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	

	glEnable(GL_CULL_FACE);

	return window;
}

void display(GLFWwindow * window, vector<D_OBject> objects, vector<Shader> shaders){
	for(D_Object object: objects){
		Shader shader = shaders[i];

		for(int i = 0; i < shader.GetUniformCount(); i += 1){
			Uniform uniform = shader.getUniform(i);
			uniform.Update();
			uniform.Bind();
		}

		GLuint ** buffers = object.GetBuffers();

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, *buffers[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBeindBuffer(GL_ARRAY_BUFFER, *buffers[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, *buffers[2]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *buffers[3]);
		glDrawElements(GL_TRIANGLES, objects.GetMesh().indices.size(), 
				GL_UNSIGNED_SHORT, 0);
	}
}


