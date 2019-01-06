#version 330 core
layout(location=0) in vec3 position;



uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Rotation;	// = model matrix


out vec3 texCoords;  // Output a texture coordinate to the fragment shader for this vertex


void main(){
	gl_Position = Projection * View * Rotation * vec4(position.x,position.y,position.z, 1.0f);

	texCoords = position;


}

