#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout (location = 2) in vec2 itexCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent; 


struct PointLight {    
    vec3 position;
    vec3 color;
	float attenuation;
}; 
uniform PointLight pointLights[6];

uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Model;	

uniform vec3 camPos;

out vec2 texCoord;  // Output a texture coordinate to the fragment shader for this vertex
out vec3 tangentFragPos;
out vec3 tangentLightPos1;
out vec3 tangentLightPos2;
out vec3 tangentLightPos3;
out vec3 tangentLightPos4;
out vec3 tangentLightPos5;
out vec3 tangentLightPos6;
out vec3 tangentCamPos;


void main(){

	gl_Position = Projection * View * Model * vec4(position.x,position.y,position.z, 1.0f);
	
	vec3 T = normalize(vec3(Model * vec4(tangent, 0.0)));
	vec3 B = normalize(vec3(Model * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(Model * vec4(normal, 0.0)));
	mat3 TBN = transpose(mat3(T, B, N));
	tangentLightPos1 = TBN * pointLights[0].position;
	tangentLightPos2 = TBN * pointLights[1].position;
	tangentLightPos3 = TBN * pointLights[2].position;
	tangentLightPos4 = TBN * pointLights[3].position;
	tangentLightPos5 = TBN * pointLights[4].position;
	tangentLightPos6 = TBN * pointLights[5].position;
    tangentCamPos  = TBN * camPos;
    tangentFragPos  = TBN * vec3(Model * vec4(position, 1.0));
	texCoord = vec2(itexCoord.x, 1.0 - itexCoord.y);  

	
}

