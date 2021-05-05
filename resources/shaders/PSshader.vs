#version 330 core

layout(location = 0) in vec3 m_vertPos;

out vec2 UV;

void main(){
		gl_Position = vec4(m_vertPos, 1);
		UV = (m_vertPos.xy + vec2(1,1))/2.0;
}
