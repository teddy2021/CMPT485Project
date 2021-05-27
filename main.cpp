#include <stdio.h>
#include <reactphysics3d/reactphysics3d.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef CON
#define CON
#include "convenience.hpp"
#endif

#ifndef SHADE
#define SHADE
#include "Shader.hpp"
#endif

#ifndef DOBJ
#define DOBJ
#include "D_Object.hpp"
#endif

#ifndef DISP
#define DISP
#include "display.hpp"
#endif

#ifndef HEADER_SIMPLE_OPENGL_IMAGE_LIBRARY
#include <SOIL2.h>
#endif

#include <boost/variant.hpp>

using namespace std;
using namespace glm;
using namespace reactphysics3d;

float FOV = 30;
vec3 position = {0, 20, 0};
vec3 direction;
vec3 v_right;

double horizontalAngle = 3.14;
double verticalAngle = 0;

float width = 1024, height = 768;

GLuint VBO;

mat4 GenMVP(Display d, D_Object obj){
	return d.GetProjection() * d.GetView() * obj.getModelMatrix();
}

int main(){

	direction = {
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	};

	v_right = {
		sin(horizontalAngle - 3.14f/2.0f),
		0.0f,
		cos(horizontalAngle - 3.14f/2.0f)
	};

	

	printf("Hello!\n");
	PhysicsCommon common;
	PhysicsWorld* world = common.createPhysicsWorld();

	Vector3 position(0, 50, 0);
	Quaternion id = Quaternion::identity();	
	const decimal timestep = 1.0f/60.0f;
	

	vector<Shader> shaders;
	vector<D_Object> objects;

	// Create Window
	Display main_window("Test", width, height);
	GLFWwindow * window = main_window.GetWindow();
	
	// Create Object and attatch shader to it
	D_Object monkey("resources/suzanne.obj", "texture.bmp" );
	Shader main_shader("shader.fs", "shader.vs");
	monkey.setShaderIdx(main_shader.GetID());

	// vertex uniforms
	shared_ptr<uni> MVP = main_shader.GetUniform("MVP");
	shared_ptr<uni> View = main_shader.GetUniform("View");
	shared_ptr<uni> Model = main_shader.GetUniform("Model");
	shared_ptr<uni> Light = main_shader.GetUniform("w_lightPosition");

	// fragment uniforms
	shared_ptr<uni> texSampler = main_shader.GetUniform("texSampler");
	shared_ptr<uni> normSampler = main_shader.GetUniform("normTexSampler");



	do{
		display(window);
	}while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
			glfwWindowShouldClose(window)==0);

	glfwTerminate();
	return 0;
}
