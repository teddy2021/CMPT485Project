
#include <stdio.h>
#include <string.h>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


#ifndef HEADER_SIMPLE_OPENGL_IMAGE_LIBRARY
#include <SOIL2.h>
#endif

#ifndef SHADE
#define SHADE
#include "Shader.hpp"
#endif

#ifndef DOBJ
#define DOBJ
#include "D_Object.hpp"
#endif

using namespace glm;
using namespace std;
using std::vector;
#ifndef CON
#define CON
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

bool getNextLine(FILE* file, char* line) {
    char word[128];
    while( 1 ) {
        
        // grab line
        fgets(line, 1024, file);
        // grab first word in line
        int res = sscanf(line, "%s", word);
        if (res == EOF) {
            return false; // EOF found
        } else if ( strcmp( word, "#" ) == 0 ) {
            // found comment, ignore and read next line
            continue;
		} else {
            return true;
        }
    }
}

bool loadModels(const char* path, std::vector<Model>& out_models) {
	printf("Loading MODELS file %s...\n", path);
    
	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}
    
    char line[1024];
    out_models.clear();
    
    int numModels = 0;
    getNextLine(file, line);
    sscanf(line, "%d", &numModels);
    out_models.resize(numModels);


    
//    float sx,sy,sz, rx,ry,rz,ra, tx,ty,tz;
//    float ar,ag,ab, dr,dg,db, sr,sg,sb,ss;
    char str[1024];
    for (int i = 0; i < numModels; ++i) {
        Model m;
        // read obj file name
		int shader_idx = 0;
		getNextLine(file, line);
		sscanf(line, "%d", &shader_idx);
		m.shader = shader_idx;
	
 		getNextLine(file, line);
        sscanf(line, "%s\n", str);
        m.objFilename.assign(str);
        
        // read transformation
        getNextLine(file, line);
        sscanf(line, "%f %f %f %f %f %f %f %f %f %f\n",
               &m.sx, &m.sy, &m.sz,
               &m.rx, &m.ry, &m.rz, &m.ra,
               &m.tx, &m.ty, &m.tz );

        // read material
        getNextLine(file, line);
        sscanf(line, "%f %f %f %f %f %f %f %f %f %f\n",
               &m.ar, &m.ag, &m.ab,
               &m.dr, &m.dg, &m.db,
               &m.sr, &m.sg, &m.sb, &m.ss );

        getNextLine(file, line);
        sscanf(line, "%s\n", str);
        m.materialTag.assign(str);
        
        getNextLine(file, line);
        sscanf(line, "%s\n", str);
        m.textureFilename.assign(str);
        
        out_models[i] = m;
    }
	return true;
}

#endif
