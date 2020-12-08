#ifndef DISPLAY
#define DISPLAY
#endif

#include <stdlib.h>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#ifndef CON
#define CON
#include "convenience.hpp"
#endif

#ifndef OGLHPP
#define OGLHPP
#include "ogl.hpp"
#endif

#ifndef DOBJ
#define DOBJ
#include "D_Object.hpp"
#endif

#ifndef SHADE
#define SHADE
#include "Shader.hpp"
#endif

extern GLuint VBO;

GLFWwindow * setup();

void display(GLFWwindow * window);
