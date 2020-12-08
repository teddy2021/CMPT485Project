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

#ifndef HEADER_SIMPLE_OPENGL_IMAGE_LIBRARY
#include <SOIL2.h>
#endif


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

	GLFWwindow * window = setup();

	setupMainLoop();
	do{
		display(window);
	}while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
			glfwWindowShouldClose(window)==0);

	glfwTerminate();
	return 0;
}
