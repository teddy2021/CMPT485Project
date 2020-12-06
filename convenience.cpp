
#ifndef CON
#define CON
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
#include <glm/gtx/quaternion.hpp>
#endif

#ifndef HEADER_SIMPLE_OPENGL_IMAGE_LIBRARY
#include <SOIL2.h>
#endif

using namespace glm;
using namespace std;

bool ReadFile(const char * file, string& out){
	
	ifstream f(file);

	bool ret = false;

	if(f.is_open()){
		string line;
		while(getline(f, line)){
			out.append(line);
			out.append("\n");

		}

		f.close();

		ret = true;
	}
	else{
		fprintf(stderr, "Provided file, %s, was not open\n", file);
		ret = false;
	}

	return ret;
}

GLuint loadBMP_custom(const char * imagepath){

	printf("Reading image %s\n", imagepath);
	unsigned char header[54];
	unsigned int datapos;
	unsigned int imagesize;
	unsigned int width, height;
	unsigned char * data;

	FILE * file = fopen(imagepath, "rb");
	if(!file){
		fprintf(stderr, "Image, %s, could not be opened\n", imagepath);
		return 0;
	}

	if( fread(header, 1, 54, file)!= 54 ){
		fprintf(stderr, "Incorrect BMP file handed in\n");
		fclose(file);
		return 0;
	}

	if( header[0] != 'B' || header[1] != 'M' ){
		fprintf(stderr, "Incorrect BMP file format\n");
		fclose(file);
		return 0;
	}

	datapos = *(int*)&header[0x0A];
	imagesize = *(int*)&header[0x22];
	width = *(int*)&header[0x12];
	height = *(int*)&header[0x16];

	if(imagesize == 0){
		imagesize = width*height*3;
	}

	if(datapos == 0){
		datapos = 54;
	}

	data = new unsigned char [imagesize];
	fread(data, 1, imagesize, file);
	fclose(file);

	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glEnable(GL_TEXTURE_2D);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, 
			GL_BGR, GL_UNSIGNED_BYTE, data);


	delete [] data;

	glGenerateMipmap(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	

	return textureID;

}

GLuint LoadShaders(const char* vert_shader, const char * frag_shader){
	GLuint vert_ID = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag_ID = glCreateShader(GL_FRAGMENT_SHADER);

	string vert_code;
	ifstream VertexShaderStream(vert_shader, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		vert_code = sstr.str();
		VertexShaderStream.close();
	}
	else{
		fprintf(stderr, "Unable to open %s.\n", vert_shader);
		getchar();
		return 0;
	}

	string frag_code;
	ifstream FragStream(frag_shader, std::ios::in);
	if(FragStream.is_open()){
		stringstream sstr;
		sstr << FragStream.rdbuf();
		frag_code = sstr.str();
		FragStream.close();
	}

	GLint res = GL_FALSE;
	int loglen;

	printf("Compiling shader: %s\n", vert_shader);
	char const * vert_src = vert_code.c_str();
	glShaderSource(vert_ID, 1, &vert_src, NULL);
	glCompileShader(vert_ID);

	glGetShaderiv(vert_ID, GL_COMPILE_STATUS, &res);
	glGetShaderiv(vert_ID, GL_INFO_LOG_LENGTH, &loglen);

	if(loglen > 0){
		std::vector<char> vert_error(loglen+1);
		glGetShaderInfoLog(vert_ID, loglen, NULL, &vert_error[0]);
		printf("%s\n", &vert_error[0]);
	}

	printf("Compiling shader: %s\n", frag_shader);
	char const * frag_src = frag_code.c_str();
	glShaderSource(frag_ID, 1, &frag_src, NULL);
	glCompileShader(frag_ID);

	glGetShaderiv(frag_ID, GL_COMPILE_STATUS, &res);
	glGetShaderiv(frag_ID, GL_INFO_LOG_LENGTH, &loglen);

	if(loglen > 0){
		std::vector<char> frag_error(loglen+1);
		glGetShaderInfoLog(frag_ID, loglen, NULL, &frag_error[0]);
		printf("%s\n", &frag_error[0]);
	}
	
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, vert_ID);
	glAttachShader(ProgramID, frag_ID);
	glLinkProgram(ProgramID);

	glGetShaderiv(ProgramID, GL_COMPILE_STATUS, &res);
	glGetShaderiv(ProgramID, GL_INFO_LOG_LENGTH, &loglen);

	if(loglen > 0){
		std::vector<char> Program_error(loglen+1);
		glGetShaderInfoLog(ProgramID, loglen, NULL, &Program_error[0]);
		printf("%s\n", &Program_error[0]);
	}

	glDetachShader(ProgramID, vert_ID);
	glDetachShader(ProgramID, frag_ID);
	glDeleteShader(vert_ID);
	glDeleteShader(frag_ID);

	return ProgramID;

}

bool loadOBJ( const char* path, 
		vector<vec3> & out_verts, 
		vector<vec2> & out_uvs, 
		vector<vec3> & out_norms ){

	printf("Loading object %s\n", path);
	FILE * file = fopen(path, "r");
	if(NULL == file){
		fprintf(stderr, "Unable to open file %s\n", path);
		return false;
	}

	vector<unsigned int> vIs, uvIs, nIs;
	vector<vec3> temp_verts;
	vector<vec2> temp_uvs;
	vector<vec3> temp_norms;

	while(1){
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if(EOF == res){
			break;
		}

		if(strcmp( lineHeader, "v" ) == 0){
			vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_verts.push_back(vertex);
		}else if(strcmp( lineHeader, "vt" ) == 0){
			vec2 tex;
			fscanf(file, "%f %f", &tex.x, &tex.y);
			temp_uvs.push_back(tex);
		}else if( strcmp( lineHeader, "vn" ) == 0){
			vec3 norm;
			fscanf(file, "%f %f %f", &norm.x, &norm.y, &norm.z);
			temp_norms.push_back(norm);
		}
		else if( strcmp( lineHeader, "f" ) == 0 ){
			string v1, v2, v3;
			unsigned int vI[3], uvI[3], nI[3];
			int matches = fscanf(file, "%u/%u/%u %u/%u/%u %u/%u/%u\n", 
					&vI[0], &uvI[0], &nI[0], 
					&vI[1], &uvI[1], &nI[1], 
					&vI[2], &uvI[2], &nI[2]);
//			printf("%s\t%u/%u/%u %u/%u/%u %u/%u/%u\n", lineHeader, 
//					vI[0], uvI[0], nI[0],
//					vI[1], uvI[1], nI[1],
//					vI[2], uvI[2], nI[2]);
			if(matches != 9){
				fprintf(stderr, "Failed to read line, perhaps wrong format?\n");
				return false;
			}else{
				for(int i = 0; i < 3; i += 1){
					vIs.push_back(vI[i]);
					uvIs.push_back(uvI[i]);
					nIs.push_back(nI[i]);
				}
			}
		}
		else{
			char garbage[1000];
			fgets(garbage, 1000, file);
		}
	}

	for( unsigned int i = 0; i < vIs.size(); i += 1 ){
		unsigned int vertIdx = vIs[i], uvIdx = uvIs[i], normIdx = nIs[i];
		
		vec3 vert = temp_verts[vertIdx - 1];
		out_verts.push_back(vert);

		vec2 uv = temp_uvs[uvIdx - 1];
		out_uvs.push_back(uv);

		vec3 norm = temp_norms[normIdx - 1];
		out_norms.push_back(norm);

	}

	fclose(file);
	return true; 
}

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint loadDDS(const char * imagepath){

	unsigned char header[124];

	FILE *fp; 
 
	/* try to open the file */ 
	fp = fopen(imagepath, "rb"); 
	if (fp == NULL){
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); 
		return 0;
	}
   
	/* verify the type of file */ 
	char filecode[4]; 
	fread(filecode, 1, 4, fp); 
	if (strncmp(filecode, "DDS ", 4) != 0) { 
		fclose(fp); 
		return 0; 
	}
	
	/* get the surface desc */ 
	fread(&header, 124, 1, fp); 

	unsigned int height      = *(unsigned int*)&(header[8 ]);
	unsigned int width	     = *(unsigned int*)&(header[12]);
	unsigned int linearSize	 = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC      = *(unsigned int*)&(header[80]);

 
	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */ 
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize; 
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char)); 
	fread(buffer, 1, bufsize, fp); 
	/* close the file pointer */ 
	fclose(fp);

//	unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4; 
	unsigned int format;
	switch(fourCC) 
	{ 
	case FOURCC_DXT1: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
		break; 
	case FOURCC_DXT3: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; 
		break; 
	case FOURCC_DXT5: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
		break; 
	default: 
		free(buffer); 
		return 0; 
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);	
	
	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
	unsigned int offset = 0;

	/* load the mipmaps */ 
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) 
	{ 
		unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize; 
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,  
			0, size, buffer + offset); 
	 
		offset += size; 
		width  /= 2; 
		height /= 2; 

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if(width < 1) width = 1;
		if(height < 1) height = 1;

	} 

	free(buffer); 

	return textureID;


}

bool is_near(float v1, float v2){
	return fabs( v1-v2 ) < 0.01f;
}

// Searches through all already-exported vertices
// for a similar one.
// Similar = same position + same UVs + same normal
bool getSimilarVertexIndex( 
	glm::vec3 & in_vertex, 
	glm::vec2 & in_uv, 
	glm::vec3 & in_normal, 
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals,
	unsigned short & result
){
	// Lame linear search
	for ( unsigned int i=0; i<out_vertices.size(); i++ ){
		if (
			is_near( in_vertex.x , out_vertices[i].x ) &&
			is_near( in_vertex.y , out_vertices[i].y ) &&
			is_near( in_vertex.z , out_vertices[i].z ) &&
			is_near( in_uv.x     , out_uvs     [i].x ) &&
			is_near( in_uv.y     , out_uvs     [i].y ) &&
			is_near( in_normal.x , out_normals [i].x ) &&
			is_near( in_normal.y , out_normals [i].y ) &&
			is_near( in_normal.z , out_normals [i].z )
		){
			result = i;
			return true;
		}
	}
	// No other vertex could be used instead.
	// Looks like we'll have to add it to the VBO.
	return false;
}

void indexVBO_slow(
	vector<vec3> & in_vertices,
	vector<vec2> & in_uvs,
	vector<vec3> & in_normals,

	vector<unsigned short> & out_indices,
	vector<vec3> & out_vertices,
	vector<vec2> & out_uvs,
	vector<vec3> & out_normals){
	// For each input vertex
	
	for ( unsigned int i=0; i<in_vertices.size(); i++ ){
		
		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex(in_vertices[i], in_uvs[i], in_normals[i],
				out_vertices, out_uvs, out_normals, index);
		if ( found ){ // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back( index );
		}else{ // If not, it needs to be added in the output data.
			out_vertices.push_back( in_vertices[i]);
			out_uvs     .push_back( in_uvs[i]);
			out_normals .push_back( in_normals[i]);
			out_indices .push_back( (unsigned short)out_vertices.size() - 1 );
		}
	}
}

GLuint text_texture;
GLuint text_vbID;
GLuint text_uvbID;
GLuint TextShader;
GLuint TextUniform;

void initText2D(const char * texPath){
	printf("Loading text texture %s\n", texPath);
	text_texture = SOIL_load_OGL_texture(
			texPath,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
			);

	if(0 == text_texture){
		fprintf(stderr, "Failed to load text texture %s; %s\n", texPath, 
				SOIL_last_result());
	}

	glGenBuffers(1, &text_vbID);
	glGenBuffers(1, &text_uvbID);

	TextShader = LoadShaders("resources/shaders/textShader.vs", 
								"resoureces/shaders/textShader.fs");

	TextUniform = glGetUniformLocation(TextShader, "texSampler");

}

void printText(const char * text, int x, int y, int size){
	unsigned int length = strlen(text);


	vector<vec2> verts;
	vector<vec2> uvs;
	for(unsigned int i = 0; i < length; i += 1){
		vec2 vert_up_left = vec2(x+i*size, y+size);
		vec2 vert_up_right = vec2(x+i*size+size, y + size);
		vec2 vert_down_right = vec2(x + i * size + size, y);
		vec2 vert_down_left = vec2(x + i * size, y);

		verts.push_back(vert_up_left);
		verts.push_back(vert_down_left);
		verts.push_back(vert_up_right);

		verts.push_back(vert_down_right);
		verts.push_back(vert_up_right);
		verts.push_back(vert_down_left);

		char character;
		float uv_x, uv_y;
		character = text[i];
		uv_x = (character%16)/16.0f;
		uv_y = (character/16)/16.0f;

		vec2 uv_up_left = vec2(uv_x, 1.0f - uv_y);
		vec2 uv_up_right = vec2(uv_x + 1.0f/16.0f, 1.0f - uv_y);
		vec2 uv_down_right = vec2(uv_x + 1.0/16.0f, 1.0f-(uv_y + 1.0f/16.0f));
		vec2 uv_down_left = vec2(uv_x, 1.0f - (uv_y + 1.0f/16.0f));

		uvs.push_back(uv_up_left);
		uvs.push_back(uv_down_left);
		uvs.push_back(uv_up_right);

		uvs.push_back(uv_down_right);
		uvs.push_back(uv_up_right);
		uvs.push_back(uv_down_left);
	}

	glBindBuffer(GL_ARRAY_BUFFER, text_vbID);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(vec2), 
			&verts[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, text_uvbID);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), 
			&uvs[0], GL_STATIC_DRAW);


	glUseProgram(TextShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text_texture);

	glUniform1i(TextUniform, 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, text_vbID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, text_uvbID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, verts.size());

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}

void cleanupText2D(){
	glDeleteBuffers(1, &text_vbID);
	glDeleteBuffers(1, &text_uvbID);
	
	glDeleteTextures(1, &text_texture);

	glDeleteProgram(TextShader);
}

void compute_tangents(
		vector<vec3> verts,
		vector<vec2> uvs,
		vector<vec3> norms,
		vector<vec3> &tans,
		vector<vec3> &bitans
		){
	for(unsigned int i = 0; i < verts.size(); i += 1){ 
		vec3 & v0 = verts[i]; 
		vec3 & v1 = verts[i+1]; 
		vec3 & v2 = verts[i+2];

		vec2 & uv0 = uvs[i]; 
		vec2 & uv1 = uvs[i+1]; 
		vec2 & uv2 = uvs[i+2];

		vec3 d_pos1 = v1-v0;
		vec3 d_pos2 = v2-v0;

		vec2 d_uv1 = uv1-uv0;
		vec2 d_uv2 = uv2-uv0;

		float r = 1.0f/(d_uv1.x * d_uv2.y - d_uv1.y * d_uv2.x);
		vec3 tan = (d_pos1* d_uv2.y - d_uv1.y * d_pos2)*r;
		vec3 bitan = (d_pos2 * d_uv1.x - d_pos1 * d_uv2.x)*r;

		tans.push_back(tan);
		tans.push_back(tan);
		tans.push_back(tan);

		bitans.push_back(bitan);
		bitans.push_back(bitan);
		bitans.push_back(bitan);
	}
	
	for(unsigned int i = 0; i < verts.size(); i +=1){
		vec3 &n = norms[i];
		vec3 &t = tans[i];
		vec3 &b = bitans[i];

		t = normalize(t - n * dot(n , t));
		if(dot(cross(n, t), b) < 0.0f){
			t *= -1;
		}
	}
}


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
){
	// For each input vertex
	for ( unsigned int i=0; i<in_vertices.size(); i++ ){

		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex(in_vertices[i], in_uvs[i], in_normals[i],     out_vertices, out_uvs, out_normals, index);

		if ( found ){ // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back( index );

			// Average the tangents and the bitangents
			out_tangents[index] += in_tangents[i];
			out_bitangents[index] += in_bitangents[i];
		}else{ // If not, it needs to be added in the output data.
			out_vertices.push_back( in_vertices[i]);
			out_uvs     .push_back( in_uvs[i]);
			out_normals .push_back( in_normals[i]);
			out_tangents .push_back( in_tangents[i]);
			out_bitangents .push_back( in_bitangents[i]);
			out_indices .push_back( (unsigned short)out_vertices.size() - 1 );
		}
	}
}

quat RotationBetweenVectors(vec3 start, vec3 dest){
	start = normalize(start);
	dest = normalize(dest);

	float cosTheta = dot(start, dest);
	vec3 rotationAxis;

	if(cosTheta < -1 + 0.001f){
		rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);
		if(length2(rotationAxis) < 0.01){
			rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);
		}
		rotationAxis = normalize(rotationAxis);
		return angleAxis(radians(180.0f), rotationAxis);
	}

	rotationAxis = cross(start, dest);

	float s = sqrt( (1+cosTheta)*2);
	float invs = 1/s;

	return quat(
			s*0.5f,
			rotationAxis.x * invs,
			rotationAxis.y * invs,
			rotationAxis.z * invs
			);
}

quat RotateTowards(quat q1, quat q2, float maxAngle){
	if(maxAngle < 0.001f){
		return q1;
	}

	float cosTheta = dot(q1, q2);

	if(cosTheta > 0.9999f){
		return q2;
	}

	if(cosTheta < 0){
		q1 *= -1.0f;
		cosTheta *= -1.0f;
	}

	float angle = acos(cosTheta);

	if(angle < maxAngle){
		return q2;
	}

	float t = maxAngle / angle;
	angle = maxAngle;

	quat res = (sin((1.0f - t) * angle) * q1 + sin(t * angle) * q2) / sin(angle);
	res = normalize(res);
	return res;
}

quat LookAt(vec3 direction, vec3 desiredUp){
	if(length2(direction) < 0.0001f){
		return quat();
	}

	vec3 right = cross(direction, desiredUp);
	desiredUp = cross(right, direction);

	quat rot1 = RotationBetweenVectors(vec3(0.0f, 0.0f, 1.0f), direction);

	vec3 newUp = rot1 * vec3(0.0f, 1.0f, 0.0f);
	quat rot2 = RotationBetweenVectors(newUp, desiredUp);

	return rot2 * rot1;
}



enum types{mat, vec, inte};

struct mesh {
	vector<vec3> vertices;
	vector<vec3> normals;
	vector<vec2> uvs;
	vector<unsigned short> indices;
}

class Display_Object{
		
	public:
		Display_Object(struct mesh meh, GLuint tex){
			mesh = meh;
			texture = tex;

			&buffers = (GLuint*)malloc(sizeof(GLuint) * 4);

			glGenBuffers(1, vb);
			glGenBuffers(1, nb);
			glGenBuffers(1, uvb);
			glGenBuffers(1, ib);

			buffers[0] = vb;
			buffers[1] = nb;
			buffers[2] = uvb;
			buffers[3] = ib;


			glBindBuffer(GL_ARRAY_BUFFER, &vb);
			glBindBuffer(GL_ARRAY_BUFFER, &uv);
			glBindBuffer(GL_ARRAY_BUFFER, &norm);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, &idx);

			glBufferData(GL_ARRAY_BUFFER, buffer.vertices.size() * sizeof(vec3), 
					vb, GL_STATIC_DRAW);
			glBufferData(GL_ARRAY_BUFFER, buffer.uvs.size() * sizeof(vec2), 
					uv, GL_STATIC_DRAW);
			glBufferData(GL_ARRAY_BUFFER, buffer.normals.size() * sizeof(vec3), 
					norm, GL_STATIC_DRAW);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
					buffer.indices.size() * sizeof(unsigned short),
					idx, GL_STATIC_DRAW);

		}

		Display_Object(const char* object_path, const char* tex_path){
			vector<vec3> verts, normals;
			vector<vec2> uvs;
			if(!loadOBJ(path, verts, uvs, normals)){
				pfrintf(stderr, "Failed to load %s\n", object_path);
				return;
			}

			struct buffers buf;
			indexVBO_slow(verts, uvs, normals, 
					indices,
				buf.vertices, buf.uvs, buf.normals);
			buffers = buf;

			texture = SOIL_load_OGL_texture(
					tex_path,
					SOIL_LOAD_AUTO,
					SOIL_CREATE_NEW_ID,
					SOIL_FLAG_MIPMAPS | 
					SOIL_FLAG_INVERT_Y | 
					SOIL_FLAG_NTSC_SAFE_RGB
					);

			glGenBuffers(1, vb);
			glGenBuffers(1, nb);
			glGenBuffers(1, uvb);
			glGenBuffers(1, ib);

			glBindBuffer(GL_ARRAY_BUFFER, &vb);
			glBindBuffer(GL_ARRAY_BUFFER, &uv);
			glBindBuffer(GL_ARRAY_BUFFER, &norm);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, &idx);

			glBufferData(GL_ARRAY_BUFFER, buffer.vertices.size() * sizeof(vec3), 
					vb, GL_STATIC_DRAW);
			glBufferData(GL_ARRAY_BUFFER, buffer.uvs.size() * sizeof(vec2), 
					uv, GL_STATIC_DRAW);
			glBufferData(GL_ARRAY_BUFFER, buffer.normals.size() * sizeof(vec3), 
					norm, GL_STATIC_DRAW);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
					buffer.indices.size() * sizeof(unsigned short),
					idx, GL_STATIC_DRAW);

		}

		struct mesh GetMesh(){
			return mesh;
		}

		GLuint* GetBuffers(){
			return &buffers[0];
		}


		GLuint getTexture(){
			return texture;
		}

		setShaderIdx(unsigned short int idx){
			shader_idx = idx;
		}

		unsigned short int getShaderIdx(){
			return shader_idx;
		}

		mat4 getModelMatrix(){
			return translate(mat4(), translation) *
				scale(mat4(), scale_factor) * 
			rotate(mat4(), rotation, rotation_axis);
		}

		void set_translation(vec3 trans){
			translation = trans;
		}

		void set_rotation(vec3 rot, vec3 rot_axis){
			rotation = rot;
			rotation_axis = rot_axis;
		}

		void set_scale(vec3 factor){
			scale_factor = factor;
		}

		void SetUniforms(){
			// TODO: implement shader uniform setting
		}

	protected:


	private:
		vec3 translation, rotation, rotation_axis, scale_factor;
		unsigned short int shader_idx;
		struct mesh;
		GLuint texture;
		GLuint vbo, uvb, nb, ib;
		GLuint[] buffers;
}
