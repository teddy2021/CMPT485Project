#ifndef SHADE
#define SHADE
#include "Shader.hpp"
#endif

#ifndef CON
#define CON
#include "convenience.cpp"
#endif

#include <string.h>
#include <stdlib.h>
using namespace std;

Shader::Shader(const char* name) {

	string vs, fs;
	std::string nm(name);
	vs = "resources/shaders/" + nm + ".vs";
	fs = "resources/shaders/" + nm + ".fs";
	const char* vss = vs.c_str();
	const char* fss = fs.c_str();

	id = LoadShaders(vss, fss);

	GLuint *program_uniforms = (GLuint*) malloc(sizeof(GLuint));
	GLenum * types = (GLenum*)  malloc(0);
	GLchar** names = (GLchar**)  malloc(0);

}

/**void Shader::GetUniformSet(int id){
	GLint count = 0, size = 0;
	GLenum type;
	GLuint i;

	const GLsizei bufsize = 32;
	GLchar name[bufsize];
	GLsizei length = 0;
	glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);


	int num_uniforms = count;

	for(i = 0; i < count; i += 1){
		printf("[%d.0] getting shader variable for shader %d and index %d\n", 
				i, id,  i);
		glGetActiveUniform(id, i, bufsize, 
				&length, &size, &type, name);
		switch(type){
			case GL_FLOAT_MAT4:{
								   uniforms.emplace_back(make_unique<UMat4>());
								   (*uniforms.back()).Init(name, i, type);
								   break;
							   }
			case GL_FLOAT_VEC3:{
								   uniforms.emplace_back(make_unique<UVec3>());
								   (*uniforms.back()).Init(name, i, type);
				break;
							   }
			case GL_FLOAT:{
							  uniforms.emplace_back(make_unique<Ufloat>());
							  (*uniforms.back()).Init(name, i, type);
				break;
						  }
			case GL_SAMPLER_2D:
			case GL_SAMPLER_2D_SHADOW:
			case GL_INT:
						  uniforms.emplace_back(make_unique<Uint>());
						  (*uniforms.back()).Init(name, i, type);

				break;
		}
	}
}
**/
int Shader::GetUniformCount(){
	return num_uniforms;
}

/**void Shader::BindUniforms(){
	for(unique_ptr<Uniform> u: uniforms){
		(*u).Bind();
	}
}
**/
