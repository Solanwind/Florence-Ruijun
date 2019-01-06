#version 330 core

layout(location = 0) in vec2 position;
layout (location = 1) in vec2 itexCoord;

out vec2 texCoord;  // Output a texture coordinate to the fragment shader for this vertex

uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Model;	

void main(){

	gl_Position = Projection * View * Model * vec4(position.x, position.y, 0.0, 1.0);
	texCoord = itexCoord;

}