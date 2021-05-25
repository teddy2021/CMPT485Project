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

class Display{
	public:
		Display(string title);
		Display(string title, float width, float height);
		void UpdateScreen();
		GLFWwindow * GetWindow();
		mat4 GetView();
		mat4 GetProjection();
	
	protected:

	private:

		static bool context;
		void Setup();
		mat4 v_matrix; // view matrix
		mat4 p_matrix; // projection matrix
		GLFWwindow * window;

};


