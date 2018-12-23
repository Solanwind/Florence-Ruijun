#version 330 core

in vec3 texCoords;

out vec4 color;

uniform samplerCube skybox; //same name as in c++ with glUniform1i()
uniform float luminosity;

void main(){
	
	color = texture(skybox,texCoords) * luminosity;

	
}


