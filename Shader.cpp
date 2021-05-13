#ifndef SHADE
#define SHADE
#include "Shader.hpp"
#endif

#include <string.h>
#include <stdlib.h>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace std;

GLuint LoadShaders(const char* vert_shader, const char * frag_shader){
	GLuint vert_ID = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag_ID = glCreateShader(GL_FRAGMENT_SHADER);

	string vert_code;
	ifstream VertexShaderStream(vert_shader, std::ios::in);
	if(VertexShaderStream.is_open()){
		stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		vert_code = sstr.str();
		VertexShaderStream.close();
	}
	else{
		fprintf(stderr, "Unable to open %s.\n", vert_shader);
		getchar();
		return 0;
	}

	string frag_code;
	ifstream FragStream(frag_shader, std::ios::in);
	if(FragStream.is_open()){
		stringstream sstr;
		sstr << FragStream.rdbuf();
		frag_code = sstr.str();
		FragStream.close();
	}

	GLint res = GL_FALSE;
	int loglen;

	printf("Compiling shader: %s\n", vert_shader);
	char const * vert_src = vert_code.c_str();
	glShaderSource(vert_ID, 1, &vert_src, NULL);
	glCompileShader(vert_ID);

	glGetShaderiv(vert_ID, GL_COMPILE_STATUS, &res);
	glGetShaderiv(vert_ID, GL_INFO_LOG_LENGTH, &loglen);

	if(loglen > 0){
		std::vector<char> vert_error(loglen+1);
		glGetShaderInfoLog(vert_ID, loglen, NULL, &vert_error[0]);
		printf("%s\n", &vert_error[0]);
	}

	printf("Compiling shader: %s\n", frag_shader);
	char const * frag_src = frag_code.c_str();
	glShaderSource(frag_ID, 1, &frag_src, NULL);
	glCompileShader(frag_ID);

	glGetShaderiv(frag_ID, GL_COMPILE_STATUS, &res);
	glGetShaderiv(frag_ID, GL_INFO_LOG_LENGTH, &loglen);

	if(loglen > 0){
		std::vector<char> frag_error(loglen+1);
		glGetShaderInfoLog(frag_ID, loglen, NULL, &frag_error[0]);
		printf("%s\n", &frag_error[0]);
	}
	
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, vert_ID);
	glAttachShader(ProgramID, frag_ID);
	glLinkProgram(ProgramID);

	glGetShaderiv(ProgramID, GL_COMPILE_STATUS, &res);
	glGetShaderiv(ProgramID, GL_INFO_LOG_LENGTH, &loglen);

	if(loglen > 0){
		std::vector<char> Program_error(loglen+1);
		glGetShaderInfoLog(ProgramID, loglen, NULL, &Program_error[0]);
		printf("%s\n", &Program_error[0]);
	}

	glDetachShader(ProgramID, vert_ID);
	glDetachShader(ProgramID, frag_ID);
	glDeleteShader(vert_ID);
	glDeleteShader(frag_ID);

	return ProgramID;
}

Uniform::Uniform(){
	id = 0;
	type = GL_FLOAT;
}

Uniform::Uniform(GLuint idv, GLenum typ): id(idv), type(typ){}

void Uniform::BindMatrix(mat2 matrix){
	glUniformMatrix2fv(id, 1, GL_FALSE, &matrix[0][0]);
}

void Uniform::BindMatrix(mat3 matrix){
	glUniformMatrix3fv(id, 1, GL_FALSE, &matrix[0][0]);
}

void Uniform::BindMatrix(mat4 matrix){
	glUniformMatrix4fv(id, 1, GL_FALSE, &matrix[0][0]);
}

void Uniform::BindVector(vec2 vec){
	glUniform2fv(id, 1, &vec[0]);
}

void Uniform::BindVector(vec3 vec){
	glUniform3fv(id, 1, &vec[0]);
}

void Uniform::BindVector(vec4 vec){
	glUniform4fv(id, 1, &vec[0]);
}

void Uniform::BindFloat(float val){
	glUniform1f(id, val);
}

void Uniform::BindFloat(float *val, int count){
	glUniform1fv(id, count, val);
}

void Uniform::BindInt(int val){
	glUniform1i(id, val);
}

void Uniform::BindInt(int *val, int count){
	glUniform1iv(id, count, val);
}

#define UMATCONST(n)\
	mat##n##_t::mat##n##_t(){\
	*data = mat##n(1);\
	}\
	\
	mat##n##_t::mat##n##_t(mat##n init){\
	*data = init;\
	}


#define UMATBIND(n)\
	void mat##n##_t::Bind(int s_id){\
		BindMatrix(*data);\
	}


#define UVECCONST(n)\
	vec##n##_t::vec##n##_t(){\
	*data = vec##n(1);\
	}\
	\
	vec##n##_t::vec##n##_t(vec##n init){\
	*data = init;\
	}


#define UVECBIND(n)\
	void vec##n##_t::Bind(int s_id){\
		BindVector(*data);\
	}

namespace Uniforms{
	UMATCONST(4);
	UMATBIND(4);
	UVECCONST(3);
	UVECBIND(3);
}

Shader::Shader(const char* name) {

	string vs, fs;
	std::string nm(name);
	vs = "resources/shaders/" + nm + ".vs";
	fs = "resources/shaders/" + nm + ".fs";
	const char* vss = vs.c_str();
	const char* fss = fs.c_str();

	id = LoadShaders(vss, fss);
	GetUniformSet();
}

void Shader::GetUniformSet(){
	GLint count = 0, size = 0;
	GLenum type;
	GLuint i;

	const GLsizei bufsize = 32;
	GLchar name[bufsize];
	GLsizei length = 0;
	glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);

	num_uniforms = count;

	for(i = 0; i < count; i += 1){
		printf("[%d.0] getting shader variable for shader %d with index %d"
				" with name %s\n", 
				i, id,  i, name);
		glGetActiveUniform(id, i, bufsize, 
				&length, &size, &type, name);

		uniforms.emplace_back(shared_ptr<Uniform>(new Uniform(i, type)));
		name_map.emplace(name, i);

	}
}

int Shader::GetUniformCount(){
	return num_uniforms;
}

shared_ptr<Uniform> Shader::GetUniform(GLchar * name){
	return uniforms.at(name_map.at(name));
}

