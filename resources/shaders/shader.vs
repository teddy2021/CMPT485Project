#version 330

layout (location=0) in vec3 m_vertPosition;
layout (location=1) in vec2 vertUV;
layout (location=2) in vec3 vertNorm;
layout (location=3) in vec3 vertTan;
layout (location=4) in vec3 vertBitan;

out vec2 UV;
out vec3 w_position;
out vec3 c_eyeDirection;
out vec3 c_lightDirection;

out vec3 L_direct_tan;
out vec3 eye_direct_tan;


uniform mat4 MVP;
uniform mat4 View;
uniform mat4 Model;
uniform vec3 w_lightPosition;

void main(){

		mat3 MV = mat3(View * inverse(transpose(Model)));

		vec3 c_vert_norm = MV * normalize(vertNorm);
		vec3 c_vert_tan = MV * normalize(vertTan);
		vec3 c_vert_bitan = MV * normalize(vertBitan);

		mat3 TBN = transpose(mat3( c_vert_tan, c_vert_bitan, c_vert_norm));

		gl_Position = MVP * vec4(m_vertPosition, 1);

		w_position = (Model * vec4(m_vertPosition, 1)).xyz;

		vec3 c_vertPosition = (View *Model * vec4(m_vertPosition, 1)).xyz;
		c_eyeDirection = vec3(0,0,0) - c_vertPosition;

		vec3 c_lightPosition = (View * vec4(c_lightDirection, 1)).xyz;
		c_lightDirection = w_lightPosition + c_eyeDirection;

		L_direct_tan = TBN * c_lightDirection;
		eye_direct_tan = TBN * c_eyeDirection;

		UV = vertUV;

}

