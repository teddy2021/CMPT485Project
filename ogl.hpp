#ifndef OGLHPP
#define OGLHPP
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
using namespace glm;
#endif

mat4 get_Vmatrix();
mat4 get_Pmatrix();
mat4 gen_MVPmatrix(GLFWwindow * window);
void mouse_move(GLFWwindow* window, double xpos, double ypos);
GLFWwindow * setup();
void setupMainLoop();
void display(GLFWwindow * window);
