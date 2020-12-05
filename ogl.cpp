#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#ifndef OGLH
#define OGLH
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#endif

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include "convenience.hpp"

#include <SOIL2.h>
#include <algorithm>
#include <unistd.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#define GL_GLEXT_PROTOTYPES
using namespace std;
using namespace glm;

// Globals
float FOV = 30;

float width = 1024;
float height = 768;

double mouse_x, mouse_y;
double horizontalAngle = 3.14;
double verticalAngle = 0;

float mouseSpeed = 0.00025f;
float speed = 3.0f;

vec3 position = {20, 0, 20};
vec3 direction;
vec3 v_right;

quat g_rotation;
// matrics and input handling
mat4 get_Vmatrix(){

	return glm::lookAt(
		position,
		position + direction,
		cross(v_right, direction)
		);
}

mat4 get_Pmatrix(){
	return glm::perspective(glm::radians(FOV), width/height, 0.1f, 100.0f);
}

mat4 gen_MVPmatrix(GLFWwindow * window){


	static float previousTime = glfwGetTime();
	float time = glfwGetTime();
	float deltaTime = time - previousTime;
	previousTime = time;

	direction = {
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
	};

	v_right = {
		sin(horizontalAngle - 3.14f/2.0f),
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	};

	//cout << to_string(v_right) << endl;

	vec3 up = cross(v_right, direction);
	
	//cout << glm::to_string(up) << endl;

	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		position += v_right * deltaTime * speed;
	}
	if(glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS){
		position -= v_right * deltaTime * speed;
	}
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
		position += up * deltaTime * speed;
	}
	if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
		position -= up * deltaTime * speed;
	}

	mat4 model = get_Mmatrix();
	mat4 view = get_Vmatrix();
	mat4 projection = get_Pmatrix();
	
	return projection * view * model;
}

void mouse_move(GLFWwindow* window, double xpos, double ypos){
	
	mouse_x = xpos/width;
	mouse_y = ypos/height;
	

	static double previousTime = glfwGetTime();
	double currentTime = glfwGetTime();

	float deltaTime = (float)(currentTime - previousTime);


	horizontalAngle += mouseSpeed * deltaTime * float(width/2 - xpos);
	verticalAngle += mouseSpeed * deltaTime * float(height/2 - ypos);

	if(verticalAngle > radians(90.0f)){
		verticalAngle = radians(90.0f);
	}
	else if(verticalAngle < radians(-90.0f) ){
		verticalAngle = radians(-90.0f);
	}

	glfwSetCursorPos(window, width/2.0f, height/2.0f);
}

void main_loop(GLFWwindow * window){


	static vec3 pos1 = vec3(-1.5f, 0.0f, 0.0f);
	static vec3 pos2 = vec3(1.5f, 0.0f, 0.0f);

	// Buffer for storing object
	GLuint vert_buffer_ID;
	glGenVertexArrays(1, &vert_buffer_ID);
	glBindVertexArray(vert_buffer_ID);

	GLuint depthProgram = LoadShaders("resources/shaders/Depth.vs", 
										"resoureces/shaders/Depth.fs");

	GLuint d_MVP = glGetUniformLocation(depthProgram, "d_MVP");

	// load texture
	GLuint Texture = SOIL_load_OGL_texture(
			"resources/textures/texture.bmp",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS|SOIL_FLAG_INVERT_Y|SOIL_FLAG_NTSC_SAFE_RGB
			);

	// indexing 
	vector<vec3> verts, normals;
	vector<vec2> uvs;
	bool res = loadOBJ("resources/objects/suzanne.obj", verts, uvs, normals);
	if(false == res){
		fprintf(stderr, "Failed to load object\n");
	}

	vector<unsigned short> indices;
	vector<vec3> i_verts, i_norms;
	vector<vec2> i_uvs;
	indexVBO_slow(verts, uvs, normals, indices, i_verts, i_uvs, i_norms);

	// create and fill buffers
	GLuint v_buffer;
	glGenBuffers(1, &v_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, v_buffer);
	glBufferData(GL_ARRAY_BUFFER, i_verts.size() * sizeof(vec3), 
			&i_verts[0], GL_STATIC_DRAW);

	GLuint uv_buffer;
	glGenBuffers(1, &uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, i_uvs.size() * sizeof(vec2), 
			&i_uvs[0], GL_STATIC_DRAW);

	GLuint norm_buffer;
	glGenBuffers(1, &norm_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, norm_buffer);
	glBufferData(GL_ARRAY_BUFFER, i_norms.size() * sizeof(vec3), 
			&i_norms[0], GL_STATIC_DRAW);

	GLuint index_buffer;
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short),
			&indices[0], GL_STATIC_DRAW);

	// render to texture
	GLuint FrameBuffer = 0;
	glGenFramebuffers(1, &FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, 
			GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

	glDrawBuffer(GL_NONE);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		fprintf(stderr, "Framebuffer status no complete\n");
	}

	GLfloat screen[] = {
		-1.0f, -1.0f,  0.0f,
		 1.0f, -1.0f,  0.0f,
		-1.0f,  1.0f,  0.0f,
		-1.0f,  1.0f,  0.0f,
		 1.0f, -1.0f,  0.0f,
		 1.0f,  1.0f,  0.0f
	};

	GLuint screen_vb;
	glGenBuffers(1, &screen_vb);
	glBindBuffer(GL_ARRAY_BUFFER, screen_vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen), screen, GL_STATIC_DRAW);
	
//  Enable when not printing to screen
//	GLuint miniID = LoadShaders("resources/shaders/PSshader.vs", "
//								resources/shaders/PSshader.fs");
//	GLuint miniTex = glGetUniformLocation(miniID, "Texture");

	GLuint shadowProgram = LoadShaders("resources/shaders/shadow.vs", 
										"resources/shaders/shadow.fs");

	GLuint texID = glGetUniformLocation(shadowProgram, "texSampler");
	GLuint MVPID = glGetUniformLocation(shadowProgram, "MVP");
	GLuint viewID = glGetUniformLocation(shadowProgram, "View");
	GLuint modelID = glGetUniformLocation(shadowProgram, "Model");
	GLuint depthBiasID = glGetUniformLocation(shadowProgram, "depthBiasMVP");
	GLuint shadowMapID = glGetUniformLocation(shadowProgram, "shadowMap");

	GLuint lightInvDirID = glGetUniformLocation(shadowProgram, "w_lightInvDir");

	int frames;
	double lastTime = glfwGetTime();

//  comment out when using mini display
	initText2D("Font/cells.png");
	char text[256] = {0};
//	int amount;
	do{
		// Load text for screen output
		double cur_time = glfwGetTime();
		float deltaTime = (float)(cur_time - lastTime);
		frames += 1;
		deltaTime = cur_time - lastTime;
		if(deltaTime >= 1){
			sprintf(text, "%.2f ms/frame", 1000.0/double(frames));
			frames = 0;
			lastTime += 1;
		}

//		quat temp = quat(180 * sin(amount/2),
//				90 * sin(amount/2),
//				1*sin(amount/2),
//				cos(amount/2)
//				);

		vec3 d_dir = pos1 - pos2;
		vec3 d_up = vec3(0.0f, 1.0f, 0.0f);

		quat orientation = normalize(LookAt(d_dir, d_up));
		//cout << "orientation: " << to_string(orientation) << endl;

		g_rotation = RotateTowards(g_rotation, orientation, 1.0f * deltaTime);
		//cout << "rotation: " << to_string(g_rotation) << endl << endl;
//		amount += 1;


		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
		glViewport(0, 0, 1024, 1024);

		//usleep(1000000);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(depthProgram);


		vec3 lightInvDir = vec3(0.5, 2, 2);
		mat4 depthPMat = ortho<float>(-10,10,-10,10,-10,20);
		mat4 depthVMat = lookAt(lightInvDir, vec3(0,0,0), vec3(0,1,0));
		mat4 depthMMat = mat4(1);
		mat4 depthMVP = depthPMat * depthVMat * depthMMat;

		glUniformMatrix4fv(d_MVP, 1, GL_FALSE, &depthMVP[0][0]);


		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, v_buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);

	
		glDisableVertexAttribArray(0);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shadowProgram);

		gen_MVPmatrix(window);
		
		mat4 View = lookAt(
				vec3(0,0,7),
				vec3(0,0,0),
				vec3(0,1,0)
					);

		mat4 Model = translate(mat4(1), pos2) * 
			mat4_cast(g_rotation) * 
			scale(mat4(1), vec3(1.0f, 1.0f, 1.0f));
		
		mat4 MVP = get_Pmatrix() * View * Model;
		
		mat4 biasMatrix = {
			vec4(0.5, 0.0, 0.0, 0.0),
			vec4(0.0, 0.5, 0.0, 0.0),
			vec4(0.0, 0.0, 0.5, 0.0),
			vec4(0.5, 0.5, 0.5, 1.0)
		};

		mat4 depthBiasMVP = biasMatrix * depthMVP;

		glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(depthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);

		glUniform3fv(lightInvDirID, 1, &lightInvDir[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(texID, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glUniform1i(shadowMapID, 1);
	
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, v_buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, norm_buffer);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);


		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		
//		comment out when using mini display
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
//		printText(text, 10, 10, 20);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);


//		enable when using mini display
//		glViewport(0,0, 256, 256);
//
//		glUseProgram(miniID);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
//
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, depthTexture);
//		glUniform1i(miniTex, 0);
//
//		glEnableVertexAttribArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, screen_vb);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//
//		glDrawArrays(GL_TRIANGLES, 0, 6);
//		glDisableVertexAttribArray(0);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	while( glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && 
			glfwWindowShouldClose(window) == 0 );


	glDeleteBuffers(1, &v_buffer);
	glDeleteBuffers(1, &uv_buffer);
	glDeleteBuffers(1, &norm_buffer);
	glDeleteBuffers(1, &index_buffer);

	glDeleteVertexArrays(1, &vert_buffer_ID);

	glfwTerminate();
}

int setup(){

	if(!glfwInit()){
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	// Set values for opengl
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create new window
	GLFWwindow * window;
	window = glfwCreateWindow(width, height, "test", NULL, NULL);
	if(NULL == window){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if(glewInit() != GLEW_OK){
		fprintf(stderr, "Failed to init GLEW\n");
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glClearColor(0, 0, 0.4f, 0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	

	glEnable(GL_CULL_FACE);

	return 0;

}

int main(){
	return setup();
}
