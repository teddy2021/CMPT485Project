#ifndef SHADE
#define SHADE
#include <type_traits>
#endif

#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>

#include<boost/variant.hpp>

using namespace glm;
using namespace std;


template <class T>
class Uniform{
	public:
		Uniform();
		Uniform(GLuint idv, string name);
		void Bind();
		void Bind(function<void(T)> f);
		void Update(function<void(T)> f);
	private:
		GLuint id; // Uniform ID
		T *data;

	protected:
		// Binding specific Uniform types
		
		// matrices
		void Bind(mat2 matrix);
		void Bind(mat3 matrix);
		void Bind(mat4 matrix);

		// Vectors
		void Bind(vec2 vec);
		void Bind(vec3 vec);
		void Bind(vec4 vec);

		// Atomic types
		void Bind(float val);
		void Bind(float* val, int count);
		void Bind(int val);
		void Bind(int * val, int count);

};

typedef Uniform<boost::variant<mat4,vec3,int,float>> uni;

class Shader{
	public:
		/**
		 * The constructor method for a shader
		 * Pre-conditions: The shader file specified exists in the 
		 * resources/shaders directory, and is correct (will not compile 
		 * otherwise)
		 * Parameters:
		 * 		name: a char * (case sensitive) which names the files the shader
		 * 		will compile. Note: the name must be the same for all shader
		 * 		types within the shader program, (e.g. for depth.fs and depth.vs
		 * 		the name depth is all that is needed and wanted)
		 * Post-conditions: The shader 
		 **/
		Shader(string name ...);

		/**
		 * A method to obtain the number of uniforms present in the shader
		 * Pre-conditions: Shader must be initialized and have an id number
		 * Post-conditions: None
		 * Returns: the count of uniforms
		 **/
		int GetUniformCount();
		
		/** 
		 * A method to get a specific uniform given its name
		 * Pre-conditions: The shader must be initialized
		 * Parameters
		 * 		name: a GLchar * (case sensitive) which names the uniform of 
		 * 		interest.
		 * Post-conditions: None
		 * Returns: a shared_ptr to the Uniform class containing the uniform's
		 * id, type, and variable (to be used when updating and binding the 
		 * uniform)
		 **/
		shared_ptr<uni> GetUniform(string name);

		/**
		 * A getter for the id of the shader instance
		 **/
		GLuint GetID();
	private:
		GLuint id;
		int num_uniforms;
		vector<shared_ptr<uni>> uniforms;
		unordered_map<string, int> name_map;

		/**
		 * A convenience method used during initialization to obtain the 
		 * names, ids, and types of shader uniforms
		 * Pre-conditions: The shader must be loaded and compiled (done during
		 * construction)
		 * Post-conditions: The instance variables of num_uniforms, uniforms,
		 * and name_map are initialized with values drawn from the shader
		 * program
		 * Returns: none
		 **/
		void GetUniformSet();
};

GLuint LoadShaders(string args ...);

void initTextD(const char * texPath);
void printText(const char * text, int x, int y, int size);
void cleanupText2D();
