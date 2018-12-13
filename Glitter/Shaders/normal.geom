#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 8) out;
in VS_OUT {
    vec3 normal;
    vec3 color;
} gs_in[];

out vec3 normal;
const float MAGNITUDE = 0.01f;


void GenerateLine(int index) {
    normal = gs_in[index].color;
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0f) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}


void main() {
	GenerateLine(0); // First vertex normal
}
