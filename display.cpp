
#include <stdlib.h>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <SOIL2.h>
#include "convenience.h"
#include "ogl.hpp"

using namespace glm;

int width = 1920, height = 1080;

vector<Display_Object> objects;
int num_shaders;
GLuint* shaders;

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
	
	depthUniforms = glGetUniformLocation(depthprogram, "d_MVP");
	
	lightInvDir = (0.5, 2, 2);

	Frame_buffer = 0;
	glGenFramebuffers(1, &Frame_buffer);
	glBindFrameBuffer(GL_FRAMEBUFFER, Frame_buffer);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, GL_DEPTH_COMPONENT_16, 1024, 1024, 0,
			GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE-MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, 
			GL_COMPARE_R_TO_TEXTURE);

	glFrameBufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffer(GL_NONE);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		fprintf(stderr, "Failed to complete framebuffer status while making",
				" depth buffer.\n");
	}
}

void computeMatrices(){
	depthProjection = ortho<float>(-10,10, -10,10, -10,20);
	depthView = lookAt(lightInvDir, vec3(0,0,0), vec3(0,1,0));
	depthModel = mat4(1);
	depthMVP = depthProjection * depthView * depthModel;
	depthBiasMVP = make_mat4(
			[0.5,0,0,0,
			0,0.5,0,0,
			0,0,0.5,0,
			0.5,0.5,0.5,1]) * dpethMVP;

}

void handleShadows(){
	computeMatrices();
	glUniformMatrix4fv(deptUniform, 1, GL_FALSE, &depthMVP[0][0]);
	
	glUseProgram(depthprogram);
	glBindFrameBuffer(GL_FRAMEBUFFER, Frame_buffer);
	glViewport(0,0,1024,1024);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(Display_Object obj : objects){
		GLuint* buffers = obj.GetBuffers();
		struct mesh = obj.GetMesh();

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, buffers); // pointer to the 0th element 
		// in the buffer array, so handing it directly is the same as buffers[0]
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers + 4);

		glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_SHORT, 0);
		glDisableVertexAttribArray(0);

	}

	glBindBuffer(GL_FRAMEBUFFER, 0);
	glViewPort(0, 0, width, height);

}

void display(){
	handleShadows();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLuint* buffers;
	struct mesh Mesh;
	GLuint texture;

	for(Display_Object obj: objects){
		buffers = obj.GetBuffers();
		Mesh = GetMesh();
		glUseProgram(shaders[obj.getShaderIdx]);
		obj.SetUniforms();

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, buffers);
		glBindBuffer(GL_ARRAY_BUFFER, buffers + 1);
		glBindBuffer(GL_ARRAY_BUFFER, buffers + 2);
		glBindBuffer(GL_ELEMENTS_ARRAY_BUFFER, buffers + 3);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawElements(GL_ELEMENT_ARRAY_BUFFER, Mesh.indices.size(), 
				GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

	}
}

void addShader(string name){
	if(num_shaders == 0){
		shaders = (GLuint *)malloc(sizeof(GLuint));
	}
	else{
		shaders = (GLuint*)realloc(shaders,sizeof(GLuint) num_shaders + 1);
	}
	num_shaders += 1;
	string vs, fs;
	vs = "resources/shaders/" + name + ".vs";
	fs = "resources/shaders/" + name + ".fs"
	shaders[num_shaders - 1] = LoadShaders(vs, fs);
}

void add_display_object(Display_Object obj){
	objects.push_back(obj);
}

void add_object_path(const char* obj_path, const char* tex_path){
	objects.push_back(Display_Object(obj_path, tex_path));
}

void add_object_buffer(struct buffers obj_buffers, GLuint texture){
	objects.push_back(Display_Object(obj_buffers, texture));
}


