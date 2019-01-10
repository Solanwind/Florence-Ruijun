#include <glad/glad.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>

class CreateVAO{
	public:

		GLuint createCubeVAO();
		GLuint BumpMappingVAO(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4, glm::vec3 nm, float repeatFactor);
		GLuint createRectVAO();

};