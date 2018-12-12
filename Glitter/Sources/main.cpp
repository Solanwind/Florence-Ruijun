// Local Headers
#include "glitter.hpp"


// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <btBulletDynamicsCommon.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

// To use stb_image, add this in *one* C++ source file.
#define STB_IMAGE_IMPLEMENTATION

//#include <GL/glut.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "createVAO.h"
#include "addComponents.hpp"

using namespace std;


// Callbacks
static bool keys[1024]; // is a key pressed or not ?
						// External static callback
						// Is called whenever a key is pressed/released via GLFW
static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mode*/);
static void mouse_button_callback(GLFWwindow* /*window*/, int button, int action, int /*mods*/);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset);

CreateVAO vao;
Camera camera(glm::vec3(0.0f, 0.0f, 1.0f));
glm::vec3 lightPos(10.2f, 1.0f, 2.0f);
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
float deltaTime = 0.0f;


void showFPS(void);
void Do_Movement();
unsigned int loadCubemap(vector<string> faces);




int main(int argc, char * argv[]) {

	// Load GLFW and Create a Window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	auto mWindow = glfwCreateWindow(mWidth, mHeight, "INFO-H-502 - Team Ruijun", nullptr, nullptr);

	// Check for Valid Context
	if (mWindow == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		return EXIT_FAILURE;
	}

	glm::mat4 myMatrix = glm::mat4(1.0);  // identify the matrix
	glm::mat4 myScaledMatrix = glm::scale(myMatrix, glm::vec3(3.0, 3.0f, 3.0f));
	glm::mat4 myRotationMatrix = glm::rotate(myScaledMatrix, 0.0f*180.0f / 3.14f, glm::vec3(1.0f, 1.0, 0.0));
	glm::mat4 myCombinationMatrix = glm::translate(myRotationMatrix, glm::vec3(0.0f, 0.0, 0.0));
	glm::mat4 View = camera.GetViewMatrix();
	glm::mat4 Projection = glm::perspective(camera.Zoom, (float)mWidth / (float)mHeight, 0.1f, 1000.0f);

	glm::mat4 myScaledMatrix_lamp = glm::scale(myMatrix, glm::vec3(0.02f, 0.02f, 0.02f));
	glm::mat4 myRotationMatrix_lamp = glm::rotate(myScaledMatrix_lamp, 1.0f, glm::vec3(1.0f, 1.0, 0.0));
	//glm::mat4 myCombinationMatrix_lamp = glm::translate(myRotationMatrix_lamp, lightPos);
	glm::mat4 myCombinationMatrix_lamp = glm::translate(myRotationMatrix_lamp, glm::vec3(0.0f, 0.0, 0.0));
	//TransformedVector = TranslationMatrix * RotationMatrix * ScaleMatrix * OriginalVector;

	// Create Context and Load OpenGL Functions
	glfwMakeContextCurrent(mWindow);
	gladLoadGL();
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	// Set the required callback functions
	glfwSetKeyCallback(mWindow, key_callback);
	glfwSetCursorPosCallback(mWindow, mouse_callback);
	glfwSetMouseButtonCallback(mWindow, mouse_button_callback);
	glfwSetScrollCallback(mWindow, scroll_callback);

	// Change Viewport
	int width, height;
	glfwGetFramebufferSize(mWindow, &width, &height);
	glViewport(0, 0, width, height);

	GLuint VAO_axis = vao.createAxisVAO();
	GLuint VAO_axis_Triangle = vao.createTriangleVAO();
	GLuint VAO_Cube = vao.createCubeVAO();
	GLuint VAO_Lamp = vao.createCubeVAO();
	GLuint VAO_Skybox = vao.createSkyboxVAO();

	///// load cubmap texture //////
	vector<string> faces;
	faces.push_back("C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Build/cubmap/posx.jpg");
	faces.push_back("C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Build/cubmap/negx.jpg");
	faces.push_back("C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Build/cubmap/posy.jpg");
	faces.push_back("C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Build/cubmap/negy.jpg");
	faces.push_back("C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Build/cubmap/posz.jpg");
	faces.push_back("C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Build/cubmap/negz.jpg");
	unsigned int cubemapTexture = loadCubemap(faces);


	Shader lightingShader = Shader(
		"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/triangle.vert",
		"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/triangle.frag");
	Shader lampShader = Shader(
		"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/lamp.vert",
		"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/lamp.frag");
	Shader skyboxShader = Shader(
		"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/skybox.vert",
		"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/skybox.frag");
	Shader cubemapShader = Shader(
		"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/cubemap.vert",
		"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/cubemap.frag");
	lightingShader.compile();
	lampShader.compile();
	skyboxShader.compile();
	cubemapShader.compile();

	Model bunny = Model("bunny2.obj");
	Model skull = Model("skull3.obj");
	


	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// shader configuration
	// --------------------
	cubemapShader.use();
	cubemapShader.setInteger("skybox", 0);
	skyboxShader.use();
	skyboxShader.setInteger("skybox", 0);

	glm::vec3 velocity(0.05f, 0.0f, 0.0f);
	// Rendering Loop
	while (glfwWindowShouldClose(mWindow) == false) {

		glm::vec3 lightColor;
		lightColor.x = sin(glfwGetTime() * 2.0f);
		lightColor.y = sin(glfwGetTime() * 0.7f);
		lightColor.z = sin(glfwGetTime() * 1.3f);


		float i = 0.0005;
		velocity.x -= i;
		glm::mat4 myCombinationMatrix = glm::translate(myRotationMatrix, velocity);
		View = camera.GetViewMatrix();
		glm::mat4 myRotationMatrix_lamp = glm::rotate(myScaledMatrix_lamp, velocity.x, glm::vec3(1.0f, 1.0, 0.0));
		//glm::mat4 myCombinationMatrix_lamp = glm::translate(myRotationMatrix_lamp, lightPos);
		glm::mat4 myCombinationMatrix_lamp = glm::translate(myRotationMatrix_lamp, glm::vec3(0.0f, 0.0, 0.0));

		showFPS();
		Do_Movement();
		// Background Fill Color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		////////////////////////////////////// Draw the triangle //////////////////////////////////
		// (Game Loop) Draw the object
		//triangleShader.use();
		//triangleShader.setVector4f("maincolor", 0.0f, 0.0f, 1.0f, 1.0f);
		//triangleShader.setMatrix4("model", myCombinationMatrix);
		//triangleShader.setMatrix4("view", View);
		//triangleShader.setMatrix4("projection", Projection);
		//glBindVertexArray(vao);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glBindVertexArray(0);


		/////////////////////////////////////// Draw the axes  ///////////////////////////////////
		//triangleShader.use();
		//triangleShader.setVector4f("maincolor", 1.0f, 0.0f, 0.0f, 0.0f);  
		//glBindVertexArray(VAO_axis);
		//glDrawArrays(GL_LINES, 0, 6);
		//glBindVertexArray(0);

		/////////////////////////////////////// Draw the cube  ///////////////////////////////////
		//GLuint texture = createTexture();
		//triangleShader.setVector4f("light", 0.0f, 0.0f, 1.0f, 1.0f);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture);
		//triangleShader.setInteger("myTexture1", 0);
		//triangleShader.setVector4f("maincolor", 1.0f, 0.0f, 1.0f, 1.0f);



		//lampShader.use();
		//lampShader.setMatrix4("model", myCombinationMatrix_lamp);
		//lampShader.setMatrix4("view", View);
		//lampShader.setMatrix4("projection", Projection);

		//glBindVertexArray(VAO_Lamp);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glBindVertexArray(0);


		//glm::mat4 bunnymodel = bunny.getmodel();

		lightingShader.use();
		lightingShader.setVector3f("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVector3f("lightColor", lightColor);
		lightingShader.setVector3f("lightPos", lightPos);
		lightingShader.setVector3f("viewPos", camera.Position);
		lightingShader.setMatrix4("model", myCombinationMatrix);
		lightingShader.setMatrix4("view", View);
		lightingShader.setMatrix4("projection", Projection);

		bunny.Draw(lightingShader);
	

		cubemapShader.use();
		cubemapShader.setMatrix4("model", myCombinationMatrix_lamp);
		cubemapShader.setMatrix4("view", View);
		cubemapShader.setMatrix4("projection", Projection);
		cubemapShader.setVector3f("cameraPos", camera.Position);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		cubemapShader.setInteger("skybox", 0);

		

		//glBindVertexArray(VAO_Cube);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glBindVertexArray(0);
		skull.Draw(cubemapShader);


		/////////////Draw skybox////////////////
		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		skyboxShader.setMatrix4("view", View);
		skyboxShader.setMatrix4("projection", Projection);
		// skybox cube
		glBindVertexArray(VAO_Skybox);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default




							  // Flip Buffers and Draw
		glfwSwapBuffers(mWindow);
		glfwPollEvents();

	}


	glfwTerminate();
	return EXIT_SUCCESS;
}

void showFPS(void) {
	static double lastTime = glfwGetTime();
	static int nbFrames = 0;

	// Measure speed
	double currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	nbFrames++;
	if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
										 // printf and reset timer
		std::cout << 1000.0 / double(nbFrames) << " ms/frame -> " << nbFrames << " frames/sec" << std::endl;
		nbFrames = 0;
		lastTime += 1.0;
	}
}

static GLuint createTexture() {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, comp;
	unsigned char* image = stbi_load("cat2.jpg", &width, &height, &comp, 0);
	// load jpg, png, ?in memory// send the image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	// generate the mipmaps
	stbi_image_free(image);
	// we don't need anymore the image data
	glBindTexture(GL_TEXTURE_2D, 0);
	// Unbind texture when done, so we won't accidently mess up our texture.
	return texture;
}

void Do_Movement()
{
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mode*/)
{
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;

	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);

	// V-SYNC
	if (keys[GLFW_KEY_U]) {
		static bool vsync = true;
		if (vsync) {
			glfwSwapInterval(1);
		}
		else {
			glfwSwapInterval(0);
		}
		vsync = !vsync;
	}
	if (keys[GLFW_KEY_LEFT]) {
		camera.ProcessKeyboard(LEFT, deltaTime);
		std::cout << "You have pressed left" << std::endl;
	}

	if (keys[GLFW_KEY_RIGHT]) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
		std::cout << "You have pressed right" << std::endl;
	}

	if (keys[GLFW_KEY_UP]) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
		std::cout << "You have pressed up" << std::endl;
	}

	if (keys[GLFW_KEY_DOWN]) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		std::cout << "You have pressed down" << std::endl;
	}

	if ((keys[GLFW_KEY_0] || keys[GLFW_KEY_KP_0])) {
		std::cout << "You have pressed 0" << std::endl;
	}

}

static void mouse_button_callback(GLFWwindow* /*window*/, int button, int action, int /*mods*/) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		keys[GLFW_MOUSE_BUTTON_RIGHT] = true;
	else
		keys[GLFW_MOUSE_BUTTON_RIGHT] = false;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		keys[GLFW_MOUSE_BUTTON_LEFT] = true;
	else
		keys[GLFW_MOUSE_BUTTON_LEFT] = false;

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
		keys[GLFW_MOUSE_BUTTON_MIDDLE] = true;
	else
		keys[GLFW_MOUSE_BUTTON_MIDDLE] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (keys[GLFW_MOUSE_BUTTON_LEFT]) {
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left        
		lastX = xpos;
		lastY = ypos;
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	if (keys[GLFW_MOUSE_BUTTON_RIGHT]) {
		std::cout << "Mouse Position : (" << xpos << ", " << ypos << ")" << std::endl;
	}
}


void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
	if (keys[GLFW_MOUSE_BUTTON_LEFT]) {
		std::cout << "Mouse Offset : " << yoffset << std::endl;
	}

}

unsigned int loadCubemap(vector<string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}