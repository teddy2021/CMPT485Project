#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 m_vertPos;

// Values that stay constant for the whole mesh.
uniform mat4 d_MVP;

void main(){
	gl_Position =  d_MVP * vec4(m_vertPos,1);
}

