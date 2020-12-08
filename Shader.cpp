#ifndef SHADE
#define SHADE
#include "Shader.hpp"
#endif

#ifndef CON
#define CON
#include "convenience.cpp"
#endif



Shader::Shader(const char* name){

	num_shaders += 1;
	string vs, fs;
	vs = "resources/shaders/" + name + ".vs";
	fs = "resources/shaders/" + name + ".fs";
	const char* vss = vs.c_str();
	const char* fss = fs.c_str();

	id = LoadShaders(vss, fss);

	GLuint *program_uniforms = (GLuint*) malloc(sizeof(GLuint));
	GLenum * types = (GLenum*)  malloc(0);
	GLchar** names = (GLchar**)  malloc(0);
}

void Shader::GetUniformSet(int id){
	GLint count = 0, size = 0;
	GLenum type;
	GLuint i;

	const GLsizei bufsize = 32;
	GLchar name[bufsize];
	GLsizei length = 0;
	glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);

	num_uniforms = count;
	uniforms = (Uniform *)malloc(sizeof(Uniform) * count);
	
	printf("[-1.%d] getting uniforms for shader %d\n", program, program);
	for(i = 0; i < count; i += 1){
		printf("[%d.0] getting shader variable for shader %d and index %d\n", 
				id, program,  i);
		glGetActiveUniform(shaders[program], i, bufsize, 
				&length, &size, &type, name);
		switch(type){
			case GL_FLOAT_MAT4:
				uniforms[i] = Umat4(name, i, type, new mat4(1));
				breka;
			case GL_FLOAT_VEC3:
				uniforms[i] = UVec3(name, i, type, new vec3(1));
				break;
			case GL_FLOAT:
				uniforms[i] = Ufloat(name, i, type, 0.0);
				break;
			case GL_SAMPLER_2D:
			case GL_SAMPLER_2D_SHADOW:
			case GL_INT:
				uniforms[i] = Uint(name, i, type, 0);
				break;
		}
	}
}

UMat4::UMat4(GLchar * nm, GLuint idv, GLenum typ, mat4 val){
	name = nm;
	id = idv;
	type = typ;
	value = val;
}

void UMat4::Update(){
	string nm(name);
	if(nm.compare("Projection") == 0){
		value = get_Pmatrix();
	}
	else if(nm.compare("View") == 0){
		value = get_Vmatrix();
	}
	else if(nm.compare("MV") == 0){
		value = gen_MVmatrix();
	}
	else if(nm.compare("MVP") == 0){
		value = gen_MVPmatrix();
	}
}

Bind(){
	glUniformMatrix4fv(id, 1, GL_FALSE, &value[0][0]);
}
