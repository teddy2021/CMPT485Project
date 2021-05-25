
#ifndef SHADE
#define SHADE
#include "Shader.hpp"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>

#include <boost/variant.hpp>

#include <GL/glew.h>
#include <glm/glm.hpp>

#ifndef HEADER_SIMPLE_OPENGL_IMAGE_LIBRARY
#include <SOIL2.h>
#endif
using namespace std;


GLuint pLoadShaders(string args ...){
	va_list shaders;
	va_start(shaders, args);
	vector<GLuint> shader_ids;

	const char * str = va_arg(shaders, const char *);
	string shader;

	GLint res = GL_FALSE;
	int loglen;

	while(str){
		shader = string(str);
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
		str = va_arg(shaders, const char *);
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

GLuint LoadShaders(string args ...){
	return pLoadShaders(args, NULL);
}

template <class T>
Uniform<T>::Uniform(){
	id = 0;
	data = (T*)malloc(sizeof(T));
}

template <class T>
Uniform<T>::Uniform(GLuint idv, string name){
	id = glGetUniformLocation(idv, name.c_str());
	data = (T*)malloc(sizeof(T));
}

template <class T>
void Uniform<T>::BindMatrix(mat2 matrix){
	glUniformMatrix2fv(id, 1, GL_FALSE, &matrix[0][0]);
}

template<class T>
void Uniform<T>::BindMatrix(mat3 matrix){
	glUniformMatrix3fv(id, 1, GL_FALSE, &matrix[0][0]);
}

template<class T>
void Uniform<T>::BindMatrix(mat4 matrix){
	glUniformMatrix4fv(id, 1, GL_FALSE, &matrix[0][0]);
}

template<class T>
void Uniform<T>::BindVector(vec2 vec){
	glUniform2fv(id, 1, &vec[0]);
}

template<class T>
void Uniform<T>::BindVector(vec3 vec){
	glUniform3fv(id, 1, &vec[0]);
}

template<class T>
void Uniform<T>::BindVector(vec4 vec){
	glUniform4fv(id, 1, &vec[0]);
}

template<class T>
void Uniform<T>::BindFloat(float val){
	glUniform1f(id, val);
}

template<class T>
void Uniform<T>::BindFloat(float *val, int count){
	glUniform1fv(id, count, val);
}

template<class T>
void Uniform<T>::BindInt(int val){
	glUniform1i(id, val);
}

template<class T>
void Uniform<T>::BindInt(int *val, int count){
	glUniform1iv(id, count, val);
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

		name_map.emplace(name, i);
		switch(type){
			case GL_FLOAT_MAT4:{
					uniforms.emplace_back(
							make_shared<Uniform<mat4>>(id, name)
							);
					break;
			}
			case GL_FLOAT_VEC3:{
					uniforms.emplace_back(
							make_shared<Uniform<vec3>>(id, name)
							);
					break;
			}
			case GL_FLOAT:{
				   uniforms.emplace_back(
						   make_shared<Uniform<float>>(id, name)
						   );
				   break;
			}
			case GL_INT:{
					uniforms.emplace_back(
							make_shared<Uniform<int>>(id, name)
							);
					break;
			}
		}
	}
}

int Shader::GetUniformCount(){
	return num_uniforms;
}

shared_ptr<Uniform<
			boost::variant<
				mat4, vec3, int, float>
				>
			> Shader::GetUniform(string name){
	return uniforms.at(name_map.at(name.c_str()));
}

GLuint Shader::GetID(){
	return id;
}


GLuint text_texture;
GLuint text_vbID;
GLuint text_uvbID;
GLuint TextShader;
GLuint TextUniform;

void initText2D(const char * texPath){
	printf("Loading text texture %s\n", texPath);
	text_texture = SOIL_load_OGL_texture(
			texPath,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
			);

	if(0 == text_texture){
		fprintf(stderr, "Failed to load text texture %s; %s\n", texPath, 
				SOIL_last_result());
	}

	glGenBuffers(1, &text_vbID);
	glGenBuffers(1, &text_uvbID);

	TextShader = LoadShaders("resources/shaders/textShader.vs", 
								"resoureces/shaders/textShader.fs");

	TextUniform = glGetUniformLocation(TextShader, "texSampler");

}

void printText(const char * text, int x, int y, int size){
	unsigned int length = strlen(text);


	vector<vec2> verts;
	vector<vec2> uvs;
	for(unsigned int i = 0; i < length; i += 1){
		vec2 vert_up_left = vec2(x+i*size, y+size);
		vec2 vert_up_right = vec2(x+i*size+size, y + size);
		vec2 vert_down_right = vec2(x + i * size + size, y);
		vec2 vert_down_left = vec2(x + i * size, y);

		verts.push_back(vert_up_left);
		verts.push_back(vert_down_left);
		verts.push_back(vert_up_right);

		verts.push_back(vert_down_right);
		verts.push_back(vert_up_right);
		verts.push_back(vert_down_left);

		char character;
		float uv_x, uv_y;
		character = text[i];
		uv_x = (character%16)/16.0f;
		uv_y = (character/16)/16.0f;

		vec2 uv_up_left = vec2(uv_x, 1.0f - uv_y);
		vec2 uv_up_right = vec2(uv_x + 1.0f/16.0f, 1.0f - uv_y);
		vec2 uv_down_right = vec2(uv_x + 1.0/16.0f, 1.0f-(uv_y + 1.0f/16.0f));
		vec2 uv_down_left = vec2(uv_x, 1.0f - (uv_y + 1.0f/16.0f));

		uvs.push_back(uv_up_left);
		uvs.push_back(uv_down_left);
		uvs.push_back(uv_up_right);

		uvs.push_back(uv_down_right);
		uvs.push_back(uv_up_right);
		uvs.push_back(uv_down_left);
	}

	glBindBuffer(GL_ARRAY_BUFFER, text_vbID);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(vec2), 
			&verts[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, text_uvbID);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), 
			&uvs[0], GL_STATIC_DRAW);


	glUseProgram(TextShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text_texture);

	glUniform1i(TextUniform, 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, text_vbID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, text_uvbID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, verts.size());

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}

void cleanupText2D(){
	glDeleteBuffers(1, &text_vbID);
	glDeleteBuffers(1, &text_uvbID);
	
	glDeleteTextures(1, &text_texture);

	glDeleteProgram(TextShader);
}

