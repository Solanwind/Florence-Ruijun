#version 330 core
layout(location=0) in vec3 position;


uniform vec4 myColorAA;		// When defined in the main 
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Rotation;	// = model matrix


out vec4 myColor;



void main(){
	gl_Position = Projection * View * Rotation * vec4(position.x,position.y,position.z, 1.0f);
	myColor = myColorAA;
	
}

