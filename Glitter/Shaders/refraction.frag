#version 330 core
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 camPos;

uniform samplerCube skybox;



void main(){

// REFRACTION OF SKYBOX
	float ratio = 1.00 / 1.52;
	vec3 incident = normalize(FragPos - camPos);
	vec3 refracted = refract(incident, normalize(Normal), ratio);
	vec4 colorRefr = vec4(texture(skybox, refracted).rgb,1.0);	
	color = colorRefr;

	
}



