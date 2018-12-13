#version 330 core
layout(location=0) in vec3 position;
layout(location = 1) in vec3 normals;
//layout(location = 1) in vec4 vertex_color;
//layout(location=1) in vec2 itexCoord;		//for texture

//uniform vec4 myColorAA;		// When defined in the main 
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Rotation;	// = model matrix


//out vec2 texCoord;  // Output a texture coordinate to the fragment shader for this vertex
//out vec4 myColor;

out vec3 FragPos;
out vec3 Normal;

void main(){
	gl_Position = Projection * View * Rotation * vec4(position.x,position.y,position.z, 1.0f);

	//texCoord = vec2(itexCoord.x, 1.0 - itexCoord.y);  // pass the texture coordinates to the fragment shader (1 - because change of coordinate, see slide session 2)
	//myColor = myColorAA;
	Normal = mat3(transpose(inverse(Rotation))) * normals;
	FragPos = vec3(Rotation * vec4(position, 1.0));	
	//myColor = vertex_color;

	
}

