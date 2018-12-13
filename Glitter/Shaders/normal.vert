#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;

uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Rotation;	// = model matrix

out VS_OUT {
    vec3 normal;
    vec3 color;
} vs_out;


void main(){
	gl_Position = Projection * View * Rotation * vec4(position, 1.0f);
	vs_out.color = normals;
	mat3 normalMatrix = mat3(transpose(inverse(View * Rotation)));
	vs_out.normal = vec3(Projection * vec4(normalMatrix * normals, 1.0));
}

