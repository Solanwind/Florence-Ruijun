#ifndef CREATEVAO_H
#define CREATEVAO_H

// System Headers
#include <glad/glad.h>

class CreateVAO {
public:
	GLuint createTriangleVAO();
	GLuint createAxisVAO();
	GLuint createCubeVAO();
	GLuint createSkyboxVAO();

};

#endif;