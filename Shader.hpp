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

class Shader{
	public:
		Shader(const char* name);
		int GetUniformCount();
		void BindUniforms();
	
	private:
		GLuint id;
		int num_uniforms;
		void GetUniformSet(int id);
};
