#version 330 core

layout (location = 0) in vec3 position;  
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 itexCoord;

out vec3 FragPos;  // world coordinates
out vec2 texCoord;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 light;

void main() {
FragPos = vec3(model * vec4(position, 1.0)); // change normal vectors to world coordinates
gl_Position = projection * view * model * vec4(position.x+0.1f, position.y, position.z, 1.0);
Normal = mat3(transpose(inverse(model))) * aNormal;  
//texCoord = vec2(itexCoord.x, 1.0-itexCoord.y);

}
