#ifndef SHADE
#define SHADE
#include "Shader.hpp"
#include <memory>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace std;

GLuint LoadShaders(string args ...){
	va_list shaders;
	va_start(shaders, args);
	vector<GLuint> shader_ids;
	string shader;

	GLint res = GL_FALSE;
	int loglen;

	while((shader = string(va_arg(shaders, char*))) != ""){
		GLint shader_id = 0;
		int idx = 0;
		if(shader.find(".vs", idx) != string::npos){
			shader_id = glCreateShader(GL_VERTEX_SHADER);
		}
		else if(shader.find(".tsc", idx) != string::npos){
			shader_id = glCreateShader(GL_TESS_CONTROL_SHADER);
		}
		else if(shader.find(".tse", idx) != string::npos){
			shader_id = glCreateShader(GL_TESS_EVALUATION_SHADER);
		}
		else if(shader.find(".gs", idx) != string::npos){
			shader_id = glCreateShader(GL_GEOMETRY_SHADER);
		}
		else if(shader.find(".fs", idx) != string::npos){
			shader_id = glCreateShader(GL_FRAGMENT_SHADER);
		}
		else if(shader.find(".cs", idx) != string::npos){
			shader_id = glCreateShader(GL_COMPUTE_SHADER);
		}
		else{
			fprintf(stderr, "Could not determine shader type for %s\n", 
					shader.c_str());
			continue;
		}

		shader_ids.push_back(shader_id);

		string shade_code;
		ifstream ShaderStream(shader, std::ios::in);
		if(ShaderStream.is_open()){
			stringstream sstr;
			sstr << ShaderStream.rdbuf();
			shade_code = sstr.str();
			ShaderStream.close();
		}
		else{
			fprintf(stderr, "Unable to open %s\n", shader.c_str());
			getchar();
			return 0;
		}


		printf("Compiling shader: %s\n", shader.c_str());
		char const * shader_src = shade_code.c_str();
		glShaderSource(shader_id, 1, &shader_src, NULL);
		glCompileShader(shader_id);

		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &res);
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &loglen);

		if(loglen > 0){
			vector<char> error(loglen+1);
			glGetShaderInfoLog(shader_id, loglen, NULL, &error[0]);
			fprintf(stderr, "%s\n", &error[0]);
		}
	}


	printf("Linking programs\n");
	GLuint ProgramID = glCreateProgram();
	for(int i = 0; i < shader_ids.size(); i += 1){
		glAttachShader(ProgramID, shader_ids[i]);
	}
	glLinkProgram(ProgramID);

	glGetShaderiv(ProgramID, GL_COMPILE_STATUS, &res);
	glGetShaderiv(ProgramID, GL_INFO_LOG_LENGTH, &loglen);

	if(loglen > 0){
		std::vector<char> Program_error(loglen+1);
		glGetShaderInfoLog(ProgramID, loglen, NULL, &Program_error[0]);
		fprintf(stderr, "%s\n", &Program_error[0]);
	}

	for(int i = 0; i < shader_ids.size(); i += 1){
		glDetachShader(ProgramID, shader_ids[i]);
		glDeleteShader(shader_ids[i]);
	}

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

Shader::Shader(string name ...) {
	id = LoadShaders(name);
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

