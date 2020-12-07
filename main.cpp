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

#include "display.hpp"

#ifndef HEADER_SIMPLE_OPENGL_IMAGE_LIBRARY
#include <SOIL2.h>
#endif

using namespace std;
using namespace glm;
using namespace reactphysics3d;

int main(){
	printf("Hello!\n");
	PhysicsCommon common;
	PhysicsWorld* world = common.createPhysicsWorld();

	Vector3 position(0, 50, 0);
	Quaternion id = Quaternion::identity();
	Transform transform(position, id);
	RigidBody* body = world->createRigidBody(transform);

	const decimal timestep = 1.0f/60.0f;
	

	for(int i = 0; i < 20; i +=1){
		world->update(timestep);

		const Transform& transform = body->getTransform();
		const Vector3& position = transform.getPosition();

		printf("Position: (%f, %f, %f)\n",
				position.x,
				position.y,
				position.z);

	}

	GLFWwindow * window = setup_display();

	int object = add_object_path("heating_plant");
	int shader = addShader("shadow");
	AssociateShader(shader, object);

	do{
		
		display();
		
	}while( glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		 glfwWindowShouldClose(window) == 0 );

	glfwTerminate();
	return 0;
}
