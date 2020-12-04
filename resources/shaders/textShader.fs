#version 330

uniform sampler2D texSampler;

in vec2 UV;

out vec4 color;

void main(){
		color = texture( texSampler, UV );
}
