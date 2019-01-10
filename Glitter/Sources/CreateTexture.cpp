#include <glad/glad.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include "CreateTexture.hpp"
#include "glitter.hpp"



GLuint CreateTexture::create2DTexture(char const * imName) {

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT); // Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);     // www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/#what-is-filtering-and-mipmapping-and-how-to-use-them
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT); // Set texture filtering

	int width_im, height_im, n;
	unsigned char* image = stbi_load(imName, &width_im, &height_im, &n, 3);
	// send the image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_im, height_im, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D); // generate the mipmaps

	stbi_image_free(image); // we don't need anymore the image data

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidently mess up our texture

	return texture;
}


GLuint CreateTexture::createSkyboxTexture() {

	std::vector<std::string> textures = {
		"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Build/posx.jpg",
		"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Build/negx.jpg",
		"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Build/posy.jpg",
		"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Build/negy.jpg" ,
		"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Build/posz.jpg",
		"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Build/negz.jpg" }; // 6 images for the 6 faces of the cube
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width_im, height_im, n;
	unsigned char* image;
	for (GLuint i = 0; i < textures.size(); i++) {
		image = stbi_load(textures[i].c_str(), &width_im, &height_im, &n, 3);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width_im, height_im, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		stbi_image_free(image);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return texture;
}


