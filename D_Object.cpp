#ifdef DOBJ
#define DOBJ
#include "D_Obj.hpp"
D_Object::D_Object( Mesh meh, GLuint tex){

	mesh = (Mesh*)malloc(sizeof(Mesh) + 
			meh.vertices.size() * sizeof(vec3) +
			meh.uvs.size() * sizeof(vec2) + 
			meh.normals.size() * sizeof(vec3) + 
			meh.indices.size() * sizeof(unsigned short));
	mesh->vertices = meh.vertices;
	mesh->uvs = meh.uvs;
	mesh->normals = meh.normals;
	mesh->indices = meh.indices;
	texture = tex;

	buffers = (GLuint**)malloc(sizeof(GLuint*)*4);
	
	buffers[0] = (GLuint*)malloc(sizeof(GLuint) * (*mesh).vertices.size());
	buffers[1] = (GLuint*)malloc(sizeof(GLuint) * (*mesh).uvs.size());
	buffers[2] = (GLuint*)malloc(sizeof(GLuint) * (*mesh).normals.size());
	buffers[3] = (GLuint*)malloc(sizeof(GLuint) * (*mesh).indices.size());
	
	GLuint *vb = buffers[0],
		   *uv = buffers[1],
		   *norm = buffers[2],
		   *idx = buffers[3];

   glGenBuffers(1, vb);
	glBindBuffer(GL_ARRAY_BUFFER, *vb);
	glBufferData(GL_ARRAY_BUFFER, (*mesh).vertices.size() * sizeof(vec3), 
			&(*mesh).vertices, GL_STATIC_DRAW);

	glGenBuffers(1, uv);
	glBindBuffer(GL_ARRAY_BUFFER, *uv);
	glBufferData(GL_ARRAY_BUFFER, (*mesh).uvs.size() * sizeof(vec2), 
			&(*mesh).uvs, GL_STATIC_DRAW);

	glGenBuffers(1, norm);
	glBindBuffer(GL_ARRAY_BUFFER, *norm);
	glBufferData(GL_ARRAY_BUFFER, (*mesh).normals.size() * sizeof(vec3), 
			&(*mesh).normals, GL_STATIC_DRAW);

	glGenBuffers(1, idx);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *idx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			(*mesh).indices.size() * sizeof(unsigned short),
			&(*mesh).indices, GL_STATIC_DRAW);

	

}

D_Object::D_Object(const char* object_path, const char* tex_path){
	vector<vec3> verts, normals;
	vector<vec2> uvs;
	if(!loadOBJ(object_path, verts, uvs, normals)){
		fprintf(stderr, "Failed to load %s\n", object_path);
		return;
	}

	Mesh m;
	
	indexVBO_slow(verts, uvs, normals, 
			m.indices,
		m.vertices, m.uvs, m.normals);

	mesh = (Mesh*)malloc(sizeof(Mesh) + 
			m.vertices.size() * sizeof(vec3) +
			m.uvs.size() * sizeof(vec2) + 
			m.normals.size() * sizeof(vec3) + 
			m.indices.size() * sizeof(unsigned short));

	mesh->vertices = m.vertices;
	mesh->uvs = m.uvs;
	mesh->normals = m.normals;
	mesh->indices = m.indices;

	texture = SOIL_load_OGL_texture(
			tex_path,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | 
			SOIL_FLAG_INVERT_Y | 
			SOIL_FLAG_NTSC_SAFE_RGB
			);

	buffers = (GLuint**)malloc(sizeof(GLuint*)*4);
	
	buffers[0] = (GLuint*)malloc(sizeof(GLuint) * (*mesh).vertices.size());
	buffers[1] = (GLuint*)malloc(sizeof(GLuint) * (*mesh).uvs.size());
	buffers[2] = (GLuint*)malloc(sizeof(GLuint) * (*mesh).normals.size());
	buffers[3] = (GLuint*)malloc(sizeof(GLuint) * (*mesh).indices.size());
	
	GLuint *vb = buffers[0],
		   *uv = buffers[1],
		   *norm = buffers[2],
		   *idx = buffers[3];


	glGenBuffers(1, vb);
	glBindBuffer(GL_ARRAY_BUFFER, *vb);
	glBufferData(GL_ARRAY_BUFFER, (*mesh).vertices.size() * sizeof(vec3), 
			&(*mesh).vertices, GL_STATIC_DRAW);

	glGenBuffers(1, uv);
	glBindBuffer(GL_ARRAY_BUFFER, *uv);
	glBufferData(GL_ARRAY_BUFFER, (*mesh).uvs.size() * sizeof(vec2), 
			&(*mesh).uvs, GL_STATIC_DRAW);

	glGenBuffers(1, norm);
	glBindBuffer(GL_ARRAY_BUFFER, *norm);
	glBufferData(GL_ARRAY_BUFFER, (*mesh).normals.size() * sizeof(vec3), 
			&(*mesh).normals, GL_STATIC_DRAW);

	glGenBuffers(1, idx);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *idx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			(*mesh).indices.size() * sizeof(unsigned short),
			&(*mesh).indices, GL_STATIC_DRAW);

}

Mesh D_Object:: GetMesh(){
	return *mesh;
}


GLuint **D_Object:: GetBuffers(){
	return buffers;
}


GLuint *D_Object:: getTexture(){
	return &texture;
}


void D_Object:: setShaderIdx(int idx){
	shader_idx = idx;
}


int D_Object:: getShaderIdx(){
	return shader_idx;
}


mat4 D_Object:: getModelMatrix(){
	mat4 id = mat4(1);
	return translate(id, translation) *
		scale(id, scale_factor) * 
	rotate(id, rotation, rotation_axis);
}


void D_Object:: set_translation(vec3 trans){
	translation = trans;
}


void D_Object:: set_rotation(float rot, vec3 rot_axis){
	rotation = rot;
	rotation_axis = rot_axis;
}


void D_Object:: set_scale(vec3 factor){
	scale_factor = factor;
}


#endif
