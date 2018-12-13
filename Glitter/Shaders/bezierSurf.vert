#version 330 core

layout(location = 0) in float t;  // between [0,1];
layout(location = 1) in float s;  // between [0,1];

uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Rotation;	// = model matrix

uniform vec3 controlPoint1;
uniform vec3 controlPoint2;
uniform vec3 controlPoint3;
uniform vec3 controlPoint4;

out vec4 color;


void main(){
	
	float u = 1 - t;
	float v = 1 - s;
	vec3 pos = u*u*u*controlPoint1 + 3*u*u*t*controlPoint2 + 3*u*t*t*controlPoint3 + t*t*t*controlPoint4;
	gl_Position = Projection * View * Rotation * vec4(pos, 1.0f);
	color = vec4(1.0,0.0,0.0,1.0);
	
}

