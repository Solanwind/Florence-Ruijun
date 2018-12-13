#version 330 core

in vec3 normal;
out vec4 color;

void main(){

	vec3 norm = normal + vec3(1.0f,1.0f,1.0f);
	norm = norm/2.0f;
	color = vec4(norm.r, norm.g, norm.b, 1.0f);
	
}


