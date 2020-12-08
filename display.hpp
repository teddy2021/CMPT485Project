#ifndef DISPLAY
#define DISPLAY
#endif

#include <stdlib.h>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#ifndef CON
#define CON
#include "convenience.hpp"
#endif

#ifndef OGLHPP
#define OGLHPP
#include "ogl.hpp"
#endif

GLFWwindow * setup_display();

void computeDepthMatrices();

void handleShadows();

void setUniform(GLuint uni, GLenum typ, GLchar* nm);

void setUniforms(int program);

void display(GLFWwindow * window);

void AssociateShader(int shader_idx, int object_idx);

void AssociateUniform(GLchar *name, void *value);

void GetUniformSet(int program, 
		vector<GLuint>& uniforms,
		vector<GLenum>& typeset,
		vector<GLchar*>& nameset);

int addShader(string name);

int add_display_object(Display_Object obj);

int add_object_path(const char* obj_path);

int add_object_buffer(Mesh mesh, GLuint texture);

int AssociateCustomVariable(GLchar name, void* var, int size);

void AssociateVariable(GLchar* name);

void AssociateVariable(GLchar * name, GLenum type);

void SetLightPosition(vec3 position);

