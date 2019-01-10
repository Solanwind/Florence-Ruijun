#include <glad/glad.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>

class CreateTexture{
	public:

		GLuint create2DTexture(char const * imName);
		GLuint createSkyboxTexture();

};