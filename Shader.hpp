#ifndef SHADE
#define SHADE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>

using namespace glm;
using namespace std;

class Uniform{
	public:
		Uniform();
		void Bind();
	
	private:
		int id;
		GLenum type;
		union {
			mat4 matrix;
			vec3 vect;
			int integer;
			float flot;
		} value;
};

class Shader{
	public:
		Shader(const char* name);
		int GetUniformCount();
	
	private:
		GLuint id;
		int num_uniforms;
		void GetUniformSet(int id);
};
