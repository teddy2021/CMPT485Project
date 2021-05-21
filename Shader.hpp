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

using namespace glm;
using namespace std;


class Uniform{
	public:
		Uniform();
		Uniform(GLuint idv, GLenum typ);
		virtual void Bind(int s_id);
	private:
		GLuint id; // Uniform ID
		GLenum type;

	protected:
		// Binding specific Uniform types
		
		// matrices
		void BindMatrix(mat2 matrix);
		void BindMatrix(mat3 matrix);
		void BindMatrix(mat4 matrix);

		// Vectors
		void BindVector(vec2 vec);
		void BindVector(vec3 vec);
		void BindVector(vec4 vec);

		// Atomic types
		void BindFloat(float val);
		void BindFloat(float* val, int count);
		void BindInt(int val);
		void BindInt(int * val, int count);

};

#define UMAT(n)\
	class mat##n##_t: public Uniform{\
		public: \
			mat##n##_t();\
			mat##n##_t(mat##n init);\
			void Bind(int s_id);\
		private:\
			unique_ptr<mat##n> data;\
};

#define UVEC(n)\
	class vec##n##_t: public Uniform{\
		public: \
				vec##n##_t();\
				vec##n##_t(vec##n init);\
				void Bind(int s_id);\
		private:\
				unique_ptr<vec##n> data;\
	};

namespace Uniforms{
	UMAT(4);
	UVEC(3);
}

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
		shared_ptr<Uniform> GetUniform(GLchar * name);
		
	private:
		GLuint id;
		int num_uniforms;
		vector<shared_ptr<Uniform>> uniforms;
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



