
#include <stdlib.h>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <SOIL2.h>
#include "convenience.h"
#include "ogl.hpp"

using namespace glm;

vector<Display_Object> objects;
int num_shaders;
GLuint* shaders;
GLuint VBO;
GLuint Frame_buffer;


void setup_display(){
	setup();
	num_shaders = 0;
	
	glGenVertexArrays(1, &VBO);
	glBindVertexArray(VBO);

	VBs = (GLuint*)malloc(0);
	UVBs = (GLuint*)malloc(0);
	NBs = (GLuint*)malloc(0);

}

void handleShadows(){

}

void display(){

}

void addShader(string name){
	if(num_shaders == 0){
		shaders = (GLuint *)malloc(sizeof(GLuint));
	}
	else{
		shaders = (GLuint*)realloc(shaders,sizeof(GLuint) num_shaders + 1);
	}
	num_shaders += 1;
	string vs, fs;
	vs = "resources/shaders/" + name + ".vs";
	fs = "resources/shaders/" + name + ".fs"
	shaders[num_shaders - 1] = LoadShaders(vs, fs);
}

void add_display_object(Display_Object obj){
	objects.push_back(obj);
}

void add_object_path(const char* obj_path, const char* tex_path){
	objects.push_back(Display_Object(obj_path, tex_path));
}

void add_object_buffer(struct buffers obj_buffers, GLuint texture){
	objects.push_back(Display_Object(obj_buffers, texture));
}


