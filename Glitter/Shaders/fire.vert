
#version 330 core
layout (location = 0) in vec3 pos;


out vec4 ParticleColor;

uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Model;	
uniform vec3 offset;
uniform vec4 color;

void main()
{
    ParticleColor = color;
    gl_Position = Projection * View * Model * vec4(pos*0.6+offset, 1.0f);
}
