#include <stdlib.h>
#include <string>
#include <unordered_map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#ifndef HEADER_SIMPLE_OPENGL_IMAGE_LIBRARY
#include <SOIL2.h>
#endif

#ifndef CON
#define CON
#include "convenience.hpp"
#endif

#ifndef OGLHPP
#define OGLHPP
#include "ogl.hpp"
#endif

using namespace std;
using namespace glm;

int width = 1920, height = 1080;

vector<Display_Object> objects;
int num_shaders;
GLuint* shaders;
vector<vector<GLuint>> shader_uniforms;
vector<vector<GLenum>> uniform_types;
vector<vector<GLchar*>> uniform_names;
unordered_map<GLchar*, void*> uniform_values;

GLuint VBO;

GLuint vert_b, uv_b, norm_b, idx_b;

GLuint Frame_buffer;
GLuint depthprogram;
GLuint depthUniform;
GLuint depthTexture;

vec3 lightInvDir;

mat4 depthProjection, depthView, depthModel, depthMVP;
mat4 depthBiasMVP;

void setup_display(){
	setup();
	num_shaders = 0;
	
	glGenVertexArrays(1, &VBO);
	glBindVertexArray(VBO);

	// Depth shader relevant details
	// The depth shader is used to create realtime shadows
	depthprogram = LoadShaders("resources/shaders/depth.vs",
										"resources/shaders/depth.fs");
	
	depthUniform = glGetUniformLocation(depthprogram, "d_MVP");
	
	lightInvDir = vec3(0.5, 2, 2);

	Frame_buffer = 0;
	glGenFramebuffers(1, &Frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, Frame_buffer);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0,
			GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, 
			GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffer(GL_NONE);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		fprintf(stderr, "Failed to complete framebuffer status while making"
				" depth buffer.\n");
	}
}

void computeDepthMatrices(){
	depthProjection = ortho<float>(-10,10, -10,10, -10,20);
	depthView = lookAt(lightInvDir, vec3(0,0,0), vec3(0,1,0));
	depthModel = mat4(1);
	depthMVP = depthProjection * depthView * depthModel;
	depthBiasMVP =  mat4(
			vec4(0.5,0,0,0),
			vec4(0,0.5,0,0),
			vec4(0,0,0.5,0),
			vec4(0.5,0.5,0.5,1)
			)* depthMVP;

}

void handleShadows(){
	computeDepthMatrices();
	glUniformMatrix4fv(depthUniform, 1, GL_FALSE, &depthMVP[0][0]);
	
	glUseProgram(depthprogram);
	glBindFramebuffer(GL_FRAMEBUFFER, Frame_buffer);
	glViewport(0,0,1024,1024);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(Display_Object obj : objects){
		GLuint* buffers = obj.GetBuffers();
		Mesh mesh = obj.GetMesh();

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, *buffers); // pointer to the 0th element 
		// in the buffer array, so handing it directly is the same as buffers[0]
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(buffers + 4));

		glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_SHORT, 0);
		glDisableVertexAttribArray(0);

	}

	glBindBuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);

}

void setUniform(GLuint uni, GLenum typ, GLchar* nm){
	void *value = (void*)uniform_values.at(nm);
	switch (typ){

		// floats
		case GL_FLOAT:
			glUniform1f(uni, *(float*)value);
			break;
		case GL_FLOAT_VEC2 :
			glUniform2fv(uni, 1, (float*)value);
			break;
		case GL_FLOAT_VEC3:
			glUniform3fv(uni, 1, (float*)value);
				break;
		case GL_FLOAT_VEC4:
			glUniform4fv(uni, 1, (float*)value);
			break;

		// ints
		case GL_INT:
			glUniform1i(uni, *(int*)value);
			break;
		case GL_INT_VEC2:
			glUniform2iv(uni, 1, (int*)value);
			break;
		case GL_INT_VEC3:
			glUniform3iv(uni, 1, (int*)value);
			break;
		case GL_INT_VEC4:
			glUniform4iv(uni, 1, (int*)value);
			break;

		// uints
		case GL_UNSIGNED_INT:
			glUniform1ui(uni, *(unsigned int*)value);
			break;
		case GL_UNSIGNED_INT_VEC2:
			glUniform2uiv(uni, 1, (unsigned int*)value);
			break;
		case GL_UNSIGNED_INT_VEC3:
			glUniform3uiv(uni, 1, (unsigned int*)value);
			break;
		case GL_UNSIGNED_INT_VEC4:
			glUniform4uiv(uni, 1, (unsigned int*)value);
			break;

		// bools
		case GL_BOOL:
			glUniform1i(uni, *(int*)value);
			break;
		case GL_BOOL_VEC2:
			glUniform2iv(uni, 1, (int*)value);
			break;
		case GL_BOOL_VEC3:
			glUniform3iv(uni, 1, (int*)value);
			break;
		case GL_BOOL_VEC4:
			glUniform4iv(uni, 1, (int*)value);
			break;

		//float square mats
		case GL_FLOAT_MAT2:
			glUniformMatrix2fv(uni, 1, GL_FALSE, (float*)value);
			break;
		case GL_FLOAT_MAT3: 
			glUniformMatrix3fv(uni, 1, GL_FALSE, (float*)value);
			break;
		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(uni, 1, GL_FALSE, (float*)value);
			break;

		//float mats
		case GL_FLOAT_MAT2x3:
			glUniformMatrix2x3fv(uni, 1, GL_FALSE, (float*)value);
			break;
		case GL_FLOAT_MAT2x4:
			glUniformMatrix2x4fv(uni, 1, GL_FALSE, (float*)value);
			break;
		case GL_FLOAT_MAT3x2:
			glUniformMatrix3x2fv(uni, 1, GL_FALSE, (float*)value);
			break;
		case GL_FLOAT_MAT3x4:
			glUniformMatrix3x4fv(uni, 1, GL_FALSE, (float*)value);
			break;
		case GL_FLOAT_MAT4x2:
			glUniformMatrix4x2fv(uni, 1, GL_FALSE, (float*)value);
			break;
		case GL_FLOAT_MAT4x3:
			glUniformMatrix4x3fv(uni, 1, GL_FALSE, (float*)value);
			break;

		//double square mats
		case GL_DOUBLE_MAT2:
			glUniformMatrix2fv(uni, 1, GL_FALSE, (float*)value);
			break;
		case GL_DOUBLE_MAT3: 
			glUniformMatrix3fv(uni, 1, GL_FALSE, (float*)value);
			break;
		case GL_DOUBLE_MAT4:
			glUniformMatrix4fv(uni, 1, GL_FALSE, (float*)value);
			break;

		//double mats
		case GL_DOUBLE_MAT2x3:
			glUniformMatrix2x3fv(uni, 1, GL_FALSE, (float*)value);
			break;
		case GL_DOUBLE_MAT2x4:
			glUniformMatrix2x4fv(uni, 1, GL_FALSE, (float*)value);
			break;
		case GL_DOUBLE_MAT3x2:
			glUniformMatrix3x2fv(uni, 1, GL_FALSE, (float*)value);
			break;
		case GL_DOUBLE_MAT3x4:
			glUniformMatrix3x4fv(uni, 1, GL_FALSE, (float*)value);
			break;
		case GL_DOUBLE_MAT4x2:
			glUniformMatrix4x2fv(uni, 1, GL_FALSE, (float*)value);
			break;
		case GL_DOUBLE_MAT4x3:
			glUniformMatrix4x3fv(uni, 1, GL_FALSE, (float*)value);
			break;

		//samplers
		case GL_SAMPLER_1D:
			glUniform1i(uni, *(int*)value);
			break;
		case GL_SAMPLER_2D:
			glUniform1i(uni, *(int*)value);
			break;
		case GL_SAMPLER_3D:
			glUniform1i(uni, *(int*)value);
			break;
		case GL_SAMPLER_CUBE:
			glUniform1i(uni, *(int*)value);
			break;


		//shadow samplers
		case GL_SAMPLER_1D_SHADOW:
			glUniform1i(uni, *(int*)value);
			break;
		case GL_SAMPLER_2D_SHADOW:
			glUniform1i(uni, *(int*)value);
			break;

			
		default:
			fprintf(stderr,
			"Unknown or unhandled type for uniform %d with name %s\n"
			"Type was %u\n", uni, nm, typ);
			return;
	}
}

void setUniforms(int program){
	vector<GLuint> uniformset = shader_uniforms.at(program);
	vector<GLenum> typeset = uniform_types.at(program);
	vector<GLchar*> nameset = uniform_names.at(program);

	for(int i = 0; i < uniformset.size(); i += 1){
		GLuint uniform = uniformset.at(i);
		GLenum type = typeset.at(i);
		GLchar* name = nameset.at(i);

	}
}

void display(){
	handleShadows();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLuint* buffers;
	Mesh mesh;
	GLuint texture;

	for(Display_Object obj: objects){
		buffers = obj.GetBuffers();
		mesh = obj.GetMesh();
		glUseProgram(shaders[obj.getShaderIdx()]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, obj.getTexture());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthTexture);

		setUniforms(obj.getShaderIdx());

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, *buffers);
		glBindBuffer(GL_ARRAY_BUFFER, *(buffers + 1));
		glBindBuffer(GL_ARRAY_BUFFER, *(buffers + 2));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(buffers + 3));
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawElements(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size(), 
				GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
}

void AssociateShader(int shader_idx, int object_idx){
	Display_Object obj = objects.at(object_idx);

	obj.setShaderIdx(shader_idx);
}

void AssosciateUniform(GLchar *name, void *value){
	std::pair<GLchar *, void *> element(name, value);
	uniform_values.insert(element);
}

void GetUniformSet(int program, 
		vector<GLuint>& uniforms,
		vector<GLenum>& typeset,
		vector<GLchar*>& nameset){
	GLint i, count, size;
	GLenum type;
	
	const GLsizei bufsize = 16;
	GLchar name[bufsize];
	GLsizei length;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);

	vector<GLuint> unifs;
	vector<GLenum> types;
	for(i = 0; i < count; i += 1){
		glGetActiveUniform(shaders[i], (GLuint)i, bufsize, 
				&length, &size, &type, name);
		uniforms.push_back(i);
		typeset.push_back(type);
		nameset.push_back(name);
	}
}

void addShader(string name){
	if(num_shaders == 0){
		shaders = (GLuint *)malloc(sizeof(GLuint));
	}
	else{
		shaders = (GLuint*)realloc(shaders,sizeof(GLuint) * num_shaders + 1);
	}
	num_shaders += 1;
	string vs, fs;
	vs = "resources/shaders/" + name + ".vs";
	fs = "resources/shaders/" + name + ".fs";
	const char* vss = vs.c_str();
	const char* fss = fs.c_str();
	shaders[num_shaders - 1] = LoadShaders(vss, fss);

	vector<GLuint> program_uniforms;
	vector<GLenum> types;
	vector<GLchar*> names;
	
	GetUniformSet(num_shaders-1, program_uniforms, types, names);
	
	shader_uniforms.push_back(program_uniforms);
	uniform_types.push_back(types);
	uniform_names.push_back(names);
}

void add_display_object(Display_Object obj){
	objects.push_back(obj);
}

void add_object_path(const char* obj_path){
	vector<Model> models;
	char location[1024];
	sprintf(location, "resources/%s/%s.models", 
			obj_path, obj_path);
	if(loadModels(location, models) != true){
		fprintf(stderr, "Failed to load from %s models file\n", obj_path);
		return;
	}



	for(Model m : models){
		Display_Object obj(m.objFilename.c_str(), m.textureFilename.c_str());

		vec3 scaler = vec3(m.sx, m.sy, m.sz);
		vec3 translation = vec3(m.tx, m.ty, m.tz);
		vec3 rotation_axis = vec3(m.rx, m.ry, m.rz);
		obj.set_scale(scaler);
		obj.set_translation(translation);
		obj.set_rotation(m.ra, rotation_axis);
	

	}

}

void add_object_buffer(Mesh mesh, GLuint texture){
	objects.push_back(Display_Object(mesh, texture));
}


