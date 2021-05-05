#version 330

layout (location = 0) in vec2 s_vertPos;
layout (location = 1) in vec2 vertexUV;

out vec2 UV;

void main(){
		vec2 H_vertPos = s_vertPos - vec2(400, 300);
		H_vertPos /= vec2(400, 300);
		gl_Position = vec4(H_vertPos, 0, 1);

		UV = vertexUV;
}
