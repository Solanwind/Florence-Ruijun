#version 330 core
layout(location=0) in vec3 position;
layout(location = 1) in vec3 normals;


uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Rotation;	// = model matrix



out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

void main(){
	gl_Position = Projection * View * Rotation * vec4(position.x,position.y,position.z, 1.0f);
	Normal = mat3(transpose(inverse(Rotation))) * normals;
	FragPos = vec3(View * Rotation * vec4(position, 1.0));	
	Color = normals;
}

