#version 330 core
layout(location=0) in vec3 position;


uniform vec4 myColorAA;		// When defined in the main 
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Model;	

out vec4 myColor;



void main(){
	gl_Position = Projection * View * Model * vec4(position, 1.0f);
	myColor = myColorAA;
	
}

