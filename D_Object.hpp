#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

using namespace glm;
using std::string;
using std::vector;

struct Model {
    // model files
    string objFilename;
    string textureFilename;
    string materialTag;
	
	int shader;
	
    // model transformation
    //    float scale[3];
    //    float axisAngleRot[4];
    //    float translation[3];
    float sx,sy,sz, rx,ry,rz,ra, tx,ty,tz;
    
    // material properties
    //    float ambient[3];
    //    float diffuse[3];
    //    float specular[3];
    //    float shininess;
    float ar,ag,ab, dr,dg,db, sr,sg,sb,ss;
    
};

struct Mesh {
	vector<vec3> vertices;
	vector<vec3> normals;
	vector<vec2> uvs;
	vector<unsigned short> indices;
};

class D_Object{
		
	public:
		D_Object(Mesh meh, GLuint tex);

		D_Object(const char* object_path, const char* tex_path);

		Mesh GetMesh();

		GLuint**GetBuffers();

		GLuint *getTexture();

		void setShaderIdx(int idx);

		int getShaderIdx();
			
		mat4 getModelMatrix();
				
		void set_translation(vec3 trans);

		void set_rotation(float rot, vec3 rot_axis);

		void set_scale(vec3 factor);

	protected:


	private:
		
		vec3 translation, rotation_axis, scale_factor;
		float rotation;
		
		int shader_idx;
		
		Mesh *mesh;
		GLuint texture;
		
		GLuint vbo, uvb, nb, ib;
		GLuint **buffers;

};
