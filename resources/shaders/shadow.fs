#version 330

in vec2 UV;
in vec3 w_position;
in vec3 c_normal;
in vec3 c_eyeDirection;
in vec3 c_lightDirection;
in vec4 shadowCoord;

layout(location = 0) out vec3 color;

uniform sampler2D texSampler;
uniform mat4 MV;
uniform vec3 w_LightPos;
uniform sampler2DShadow shadowMap;

vec2 poissonDisk[16] = vec2[](
	vec2( -0.94201624, -0.39906216 ), 
	vec2( 0.94558609, -0.76890725 ), 
	vec2( -0.094184101, -0.92938870 ), 
	vec2( 0.34495938, 0.29387760 ), 
	vec2( -0.91588581, 0.45771432 ), 
	vec2( -0.81544232, -0.87912464 ), 
	vec2( -0.38277543, 0.27676845 ), 
	vec2( 0.97484398, 0.75648379 ), 
	vec2( 0.44323325, -0.97511554 ), 
	vec2( 0.53742981, -0.47373420 ), 
	vec2( -0.26496911, -0.41893023 ), 
	vec2( 0.79197514, 0.19090188 ), 
	vec2( -0.24188840, 0.99706507 ), 
	vec2( -0.81409955, 0.91437590 ), 
	vec2( 0.19984126, 0.78641367 ), 
	vec2( 0.14383161, -0.14100790 ) 
);

float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed, i);
	float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
	return fract(sin(dot_product) * 43758.5453);
}

void main(){

		// main light component

		vec3 lightColor = vec3(1, 1, 1);
		float lightPower = 1.0;
		vec3 light = lightColor * lightPower;

		vec3 matDifColor = texture(texSampler, UV).rgb;
		vec3 matAmbColor = vec3(0.1, 0.1, 0.1) * matDifColor;
		vec3 matSpecColor = vec3(0.3, 0.3, 0.3);

		vec3 n = normalize( c_normal );
		vec3 l = normalize( c_lightDirection ); 

		//float distance = length(w_lightPosition - w_position);		
		//float sq_dist = distance * distance;

		// specular light component
		vec3 e = normalize( c_eyeDirection  );
		vec3 r = reflect(-l, n);

		float cosAlpha = clamp( dot(e, r), 0, 1 );
		float cosTheta = clamp(dot( n, l ), 0,1);

		float visibility = 1.0;

		//float bias = 0.005;
		float bias = 0.005*tan(acos(cosTheta));
		bias = clamp(bias, 0, 0.1);

		for(int i = 0; i < 4; i += 1){
			int index = int(16.0*random(gl_FragCoord.xyy, i))%16;
			vec2 xy = shadowCoord.xy  + poissonDisk[i]/700.0;
			float z = (shadowCoord.z - bias)/shadowCoord.w;
			visibility -= 0.2*(1.0-texture( shadowMap, vec3(xy, z )));
		};

		
	
		color = /**(
		matAmbColor + 		
		visibility * matDifColor * light * cosTheta +
		visibility * matSpecColor * light * pow(cosAlpha, 5)
		);**/
			vec3(1,1,1);

}
