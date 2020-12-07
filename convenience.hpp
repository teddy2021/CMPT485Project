#ifndef CON
#define CON
#endif

#include <stdio.h>
#include <string.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/quaternion.hpp>

using namespace glm;
using namespace std;
using std::vector;

bool ReadFile(const char* file, string& out);

GLuint loadBMP_custom(const char * imagepath);

GLuint LoadShaders(const char* vert_shader, const char * frag_shader);

bool loadOBJ( const char* path, 
		vector<vec3> & out_verts, 
		vector<vec2> & out_uvs, 
		vector<vec3> & out_norms );

GLuint loadDDS(const char * path);

bool is_near(float v1, float v2);

bool getSimilarVertexIndex( 
	vec3 & in_vertex, 
	vec2 & in_uv, 
	vec3 & in_normal, 
	vector<vec3> & out_vertices,
	vector<vec2> & out_uvs,
	vector<vec3> & out_normals,
	unsigned short & result
);


void indexVBO_slow(
	vector<vec3> & in_vertices,
	vector<vec2> & in_uvs,
	vector<vec3> & in_normals,

	vector<unsigned short> & out_indices,
	vector<vec3> & out_vertices,
	vector<vec2> & out_uvs,
	vector<vec3> & out_normals);


void initText2D(const char * texPath);

void printText(const char * text, int x, int y, int size);

void cleanupText2D();

void compute_tangents(
		vector<vec3> verts,
		vector<vec2> uvs,
		vector<vec3> norms,
		vector<vec3> &tans,
		vector<vec3> &bitans
		);

void indexVBO_TBN(
	vector<vec3> & in_vertices,
	vector<vec2> & in_uvs,
	vector<vec3> & in_normals,
	vector<vec3> & in_tangents,
	vector<vec3> & in_bitangents,
	vector<unsigned short> & out_indices,
	vector<vec3> & out_vertices,
	vector<vec2> & out_uvs,
	vector<vec3> & out_normals,
	vector<vec3> & out_tangents,
	vector<vec3> & out_bitangents
);

quat RotationBetweenVectors(vec3 start, vec3 dest);
quat RotateTowards(quat q1, quat q2, float maxAngle);
quat LookAt(vec3 direction, vec3 desiredUp);

struct Model {
    // model files
    std::string objFilename;
    std::string textureFilename;
    std::string materialTag;
	
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

#ifndef DISPLAY_OBJ
#define DISPLAY_OBJ
class Display_Object{
		
	public:
		Display_Object(Mesh meh, GLuint tex);

		Display_Object(const char* object_path, const char* tex_path);

		Mesh GetMesh();

		GLuint* GetBuffers();

		GLuint getTexture();

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
		GLuint *buffers;

};
#endif

bool loadModels(const char* path, std::vector<Model>& out_models);

bool getNextLine(FILE* file, char* line);
