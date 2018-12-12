#include "createVAO.h"


GLuint CreateVAO::createTriangleVAO() {

	GLfloat TriangleVertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f,0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f };

	GLuint TriangleVAO, TrianglePosVBO;  // handler to our VBO object
	glGenVertexArrays(1, &TriangleVAO);

	// Bind the Vertex Array Object
	glBindVertexArray(TriangleVAO);

	// Copy our vertices array in a buffer for OpenGl to use
	glGenBuffers(1, &TrianglePosVBO);
	// ask OpenGL to create one object 
	glBindBuffer(GL_ARRAY_BUFFER, TrianglePosVBO);
	// notify OGL that the object is of VBO type 
	// send the data to the GPU¡¯s memory 
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVertices), TriangleVertices, GL_STATIC_DRAW);

	// Then set our vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);  // needed to activate the pointer!!! At location 0 (same as index)

								   //glGenBuffers(1, &TriangleColorsVBO); // second VBO 
								   //glBindBuffer(GL_ARRAY_BUFFER, TriangleColorsVBO);
								   //glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);  // needed to activate the pointer!!! At location 0 (same as index)


								   // Unbind the VAO
	glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind this object


	return TriangleVAO;
}

GLuint CreateVAO::createAxisVAO() {

	// Axis: 2 vertices per line
	GLfloat vertices_axis[] = {
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,				//origin + 1,0,0 (x axis)
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f };

	GLuint VAO_axis, VBO_axis;
	glGenVertexArrays(1, &VAO_axis);	// Create a VAO (pointer)
	glBindVertexArray(VAO_axis);		// Use the VAO

										// VBO, for axis
										//Copy our vertices array in a buffer for OpenGL to use - Creation of VBO
	glGenBuffers(1, &VBO_axis);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_axis), vertices_axis, GL_STATIC_DRAW);
	// How to interpret datas
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Unbind the VAO (say that we don't use it anymore...)
	glBindVertexArray(0);

	return VAO_axis;
}


GLuint CreateVAO::createCubeVAO() {

	static const GLfloat g_vertex_buffer_data[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	GLuint VAO_Cube, VBO_Cube;

	glGenVertexArrays(1, &VAO_Cube);	// Create a VAO (pointer)
	glBindVertexArray(VAO_Cube);
	glGenBuffers(1, &VBO_Cube);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);



	glVertexAttribPointer(
		0,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		6 * sizeof(GLfloat),                                // stride
		(void*)0                         // array buffer offset
	);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);


	return VAO_Cube;
}


GLuint CreateVAO::createSkyboxVAO() {

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	return skyboxVAO;
}