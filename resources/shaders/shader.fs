#version 330

in vec2 UV;
in vec3 w_position;
in vec3 c_normal;
in vec3 c_eyeDirection;
in vec3 c_lightDirection;

in vec3 L_direct_tan;
in vec3 eye_direct_tan;

out vec3 FragColor;

uniform sampler2D texSampler;
uniform sampler2D normTexSampler;
uniform vec3 w_lightPosition;

void main(){

		// main light component

		vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
		float lightPower = 40.0f;

		vec3 matDifColor = texture(texSampler, UV).rgb;
		vec3 matSpecColor = vec3(0.2f,0.2f,0.2f);
		vec3 matAmbColor = vec3(0.001f, 0.001f, 0.001f) * matDifColor;

		vec3 n = normalize(texture(normTexSampler, UV).rgb*2.0 - 1.0);
		vec3 l = normalize(L_direct_tan); 

		float distance = length(w_lightPosition - w_position);		
		float sq_dist = distance * distance;

		// specular light component
		vec3 e = normalize( eye_direct_tan  );
		vec3 r = reflect(-l, n);

		float cosAlpha = clamp( dot(e, r), 0, 1 );
		float cosTheta = clamp(dot( n, l ), 0,1);

		FragColor = 
		//matAmbColor + 
		(matDifColor * lightColor * lightPower * cosTheta/sq_dist) +
		(matSpecColor * lightColor * lightPower * pow(cosAlpha, 5)/sq_dist);
}
