#include <stdlib.h>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#ifndef SOIL
#define HEADER_SIMPLE-OPENGL_IMAGE_LIBRARY
#include <SOIL2.h>
#endif

#ifndef CON
#define CON
#include "convenience.h"
#endif

#ifndef OGLHPP
#define OGLHPP
#include "ogl.hpp"
#endif


void setup_display();

void computeDepthMatrices();

void handleShadows();

void setUniform(GLuint uni, GLenum typ, GLchar[] nm);


void setUniforms(int program);

void display();

void AssociateShader(int shader_idx, int object_idx);

void AssosciateUniform(GLchar[] name, void* value);

void GetUniformSet(int program, 
		vector<GLuint>& uniforms
		vector<GLenum>& typeset,
		vector<GLchar[]>& nameset);

void addShader(string name);

void add_display_object(Display_Object obj);

void add_object_path(const char* obj_path);

void add_object_buffer(struct buffers obj_buffers, GLuint texture);

