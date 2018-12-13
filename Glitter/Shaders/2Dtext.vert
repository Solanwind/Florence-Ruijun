#version 330 core

layout(location = 0) in vec3 position;
layout (location = 1) in vec2 itexCoord;


uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Rotation;	// = model matrix


out vec2 texCoord;  // Output a texture coordinate to the fragment shader for this vertex


void main(){

	gl_Position = Projection * View * Rotation * vec4(position.x,position.y,position.z, 1.0f);
	texCoord = vec2(itexCoord.x, 1.0 - itexCoord.y);  

}