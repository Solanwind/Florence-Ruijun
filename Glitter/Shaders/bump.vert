#version 330 core

layout(location = 0) in vec3 position;
layout (location = 1) in vec2 itexCoord;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bitangent; 


uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Rotation;	// = model matrix


uniform vec3 lightPos;
uniform vec3 camPos;

out vec2 texCoord;  // Output a texture coordinate to the fragment shader for this vertex
out vec3 tangentFragPos;
out vec3 tangentLightPos;
out vec3 tangentCamPos;


void main(){

	gl_Position = Projection * View * Rotation * vec4(position.x,position.y,position.z, 1.0f);
	vec3 normals = cross(tangent,bitangent);


	vec3 T = normalize(vec3(Rotation * vec4(tangent, 0.0)));
	vec3 B = normalize(vec3(Rotation * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(Rotation * vec4(normals, 0.0)));
	//mat3 TBN = transpose(mat3(T, B, N));
	mat3 TBN = mat3(1.0f);
	tangentLightPos = TBN * lightPos;
    tangentCamPos  = TBN * camPos;
    tangentFragPos  = TBN * vec3(Rotation * vec4(position, 1.0));
	texCoord = vec2(itexCoord.x, 1.0 - itexCoord.y);  

	
}

