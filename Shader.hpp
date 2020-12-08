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
	   	Uniform(GLchar * nm, GLuint idv, GLenum typ);
		virtual void Update();
		virtual void Bind();
	
	private:
		GLchar * name;
		GLuint id;
		GLenum typ;
};

using UP = std::unique_ptr<Uniform>;

class Shader{
	public:
		Shader(const char* name);
		Uniform * GetUniforms();
		Uniform  GetUniform(int index);
		int GetUniformCount();
	private:
		GLuint id;
		UP* uniforms;
		int num_uniforms;

		void GetUniformSet(int id);
};

class UMat4: Uniform{
	public:
		UMat4(GLchar * nm, GLuint idv, GLenum typ, mat4 val);
		void Update() override;
		void Bind() override;
	private: 
		mat4 value;
};

class UVec3: Uniform{
	public:
		void Update() override;
		void Bind() override;
	private:
		vec3 value;
};

class Uint: Uniform{
	public:
		void Update() override;
		void Bind() override;
	private:
		int value;
};

class Ufloat: Uniform{
	public:
		void Update() override;
		void Bind() override;
	private:
		float value;
};
