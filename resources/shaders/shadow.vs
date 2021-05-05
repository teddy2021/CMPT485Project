#version 330

layout (location=0) in vec3 m_vertPosition;
layout (location=1) in vec2 vertUV;
layout (location=2) in vec3 m_vertNorm;

out vec2 UV;
out vec3 w_position;
out vec3 c_normal;
out vec3 c_eyeDirection;
out vec3 c_lightDirection;
out vec4 shadowCoord;


uniform mat4 MVP;
uniform mat4 View;
uniform mat4 Model;
uniform vec3 w_lightPosition;
uniform mat4 depthBiasMVP;

void main(){

		vec3 w_lightInvDir = w_lightPosition - m_vertPosition;
		gl_Position = MVP * vec4(m_vertPosition, 1);

		shadowCoord = depthBiasMVP * vec4(m_vertPosition, 1.0);

		w_position = (Model * vec4(m_vertPosition, 1)).xyz;

		c_eyeDirection = vec3(0, 0, 0) - ( View * Model * vec4(m_vertPosition, 1) ).xyz;

		c_lightDirection = (View * vec4(w_lightInvDir,0)).xyz;

		c_normal = (View * Model * vec4(m_vertNorm, 0)).xyz;

		UV = vertUV;
}

