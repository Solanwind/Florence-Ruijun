// Local Headers
#define STB_IMAGE_IMPLEMENTATION
#include "glitter.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"
#include "Model.hpp"

#include "btBulletDynamicsCommon.h"
#include <stdio.h>
#include "Camera.hpp"
#include "CreateVAOs.hpp"
#include "addComponents.hpp"
#include "CreateTexture.hpp"


struct Particle {
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 speed = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	GLfloat lifetime = 1.0f;
};


// Callbacks
static bool keys[1024]; // is a key pressed or not ?
						// External static callback
						// Is called whenever a key is pressed/released via GLFW
static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mode*/);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset);

Camera camera(glm::vec3(0.0f, 15.0f, 70.0f));
GLfloat lastX = 400, lastY = 300;
bool firstThrow = true;
bool ballView = false;


void showFPS(void);
void Do_Movement();


CreateVAO vaos;
CreateTexture textures;
AddComponents components;

GLuint FirstUnusedParticle();

void newParticle(Particle &particle, glm::vec3 objPos);
btTriangleMesh  * ObjToCollisionShape(std::string inputFile);
//glm::mat4 bulletMatToOpenGLMat(btScalar	bulletMat[16]);



int unusedParticle = 0;			// No more life
GLuint nbParticles = 300;
std::vector<Particle> particles;
int lastUsedParticle = 0;

float deltaTime = 0.0f;


glm::mat4 rot = glm::mat4(1.0f);	// identity matrix
float rot_x = 0.0f, rot_y = 0.0f;



btScalar matrixBall[16];
bool throwBall = false;








int main(int argc, char * argv[]) {

	
	// Load GLFW and Create a Window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	auto mWindow = glfwCreateWindow(mWidth, mHeight, "BOWLING", nullptr, nullptr);

	// Check for Valid Context
	if (mWindow == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		return EXIT_FAILURE;
	}

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





	////////////////// PARTICLES //////////////////////////////////////////

	for (GLuint i = 0; i < nbParticles; ++i) {
		particles.push_back(Particle());
	}
	float dt = 0.01;


	///////////////////////// LOADING MODELS ///////////////////////////////////////////
	Model ball("bowlingBall.obj");
	Model sphere("sphere.obj");
	Model pin("pin3.obj");

#pragma region MyRegion



	//////////////////////////// PHYSICS INITIALIZATIION ///////////////////////////////////

	components.init();

	///////////// Static rigid body - the bowling alley

	components.addBowlingAlley();

	/////////// Static rigid body - the ground of the skybox

	components.addGround();

	///////// Next rigid bodies - Pins

	components.addPins();
#pragma endregion

	/////////////////////////// SOME PARAMETERS /////////////////////////////////////////

	bool renderBall = false;
	double time;
	float luminosity;

	//////// Light
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.0f,  8.0f,  15.0f),
		glm::vec3(0.0f, 8.0f, -4.0f),
		glm::vec3(3.0f,  3.0f, 40.0f),
		glm::vec3(19.0f,  24.0f, 35.0f),
		glm::vec3(19.0f,  24.0f, -14.0f),
		glm::vec3(0.0f,  6.0f, 25.0f) };
	glm::vec3 pointLightColors[] = {
		glm::vec3(1.0f,  1.0f,  1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f,  1.0f, 0.0f),
		glm::vec3(0.0f,  0.0f, 1.0f) };
	float pointLightAttenuations[] = { 0, 1, 3, 20, 7, 3 };

	//////// Object colors
	glm::vec4 groundColor = glm::vec4(1.0, 0.7, 0.4, 1.0);
	glm::vec3 ballColor = glm::vec3(0.0, 0.5, 0.0);
	glm::vec3 pinColor = glm::vec3(0.8, 0.8, 0.8);

	//////// Bump mapping ground
	glm::vec3 pos1G(-15.0f, 0.0f, 45.0f);
	glm::vec3 pos2G(-15.0f, 0.0f, -15.0f);
	glm::vec3 pos3G(15.0f, 0.0f, -15.0f);
	glm::vec3 pos4G(15.0f, 0.0f, 45.0f);
	glm::vec3 nmG(0.0f, 1.0f, 0.0f);

	//////// Bump mapping wall
	glm::vec3 pos1W(20.01f, -100.0f, 100.0f);
	glm::vec3 pos2W(20.01f, -100.0f, -100.0f);
	glm::vec3 pos3W(20.01f, 100.0f, -100.0f);
	glm::vec3 pos4W(20.01f, 100.0f, 100.0f);
	glm::vec3 nmW(-1.0f, 0.0f, 0.0f);

	//////// Mirror camera matrices (to render scene from the mirror point of view)
	glm::mat4 rotM = glm::rotate(glm::mat4(1), 4.7f, { 0, 1, 0 });
	Camera cameraMirror(glm::vec3(20.0f, 10.0f, 0.0f));
	glm::mat4 CameraMatrixMirror = rotM * cameraMirror.GetViewMatrix();
	glm::mat4 ProjectionMatrixMirror = glm::perspective(cameraMirror.Zoom, (float)mWidth / (float)mHeight, 0.1f, 1000.0f);
	glm::vec3 cameraPositionMirror = cameraMirror.Position;

	//////// Camera matrices (principal view)
	glm::mat4 CameraMatrix, ProjectionMatrix;
	glm::vec3 cameraPosition;

	//////// Ball view
	/////////////******************
	/*btVector3 ballPos = btVector3(0, 3, 44);*/
	btVector3 ballPos;

	//////// Object matrices (rotation, translation, scaling)
	glm::mat4 matBall, matPin1, matPin2, matPin3, matPin4, matPin5, matPin6, matPin7, matPin8, matPin9, matPin10;
	glm::mat4 scaleMatrixGround = glm::scale(glm::mat4(1.f), glm::vec3(15, 1, 30));
	glm::mat4 transMatrixGround = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 15));
	glm::mat4 scaleMatrixSp = glm::scale(glm::mat4(1.f), glm::vec3(5, 5, 5));
	glm::mat4 transMatrixSp = glm::translate(glm::mat4(1.f), glm::vec3(-5, 24, -15));
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0, 1.01, 0));
	glm::mat4 scaleMatrixSkybox = glm::scale(glm::mat4(1.f), glm::vec3(100, 100, 100));
	glm::mat4 rotMirror = glm::rotate(glm::mat4(1.0f), 1.5708f, glm::vec3(0, 1, 0));  //rotation of 90 degree
	glm::mat4 scaleMirror = glm::scale(glm::mat4(1.0f), glm::vec3(15, 15, 15));
	glm::mat4 transMirror = glm::translate(glm::mat4(1.0f), cameraPositionMirror);
	glm::mat4 translationMatrixLamp = glm::translate(glm::mat4(1.f), pointLightPositions[4]);
	glm::mat4 translationMatrixLamp2 = glm::translate(glm::mat4(1.f), pointLightPositions[3]);
	glm::mat4 scaleLamp3 = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));
	glm::mat4 translationMatrixLamp3;


	////////////////////////////// CREATE NEEDED VAOS, TEXTURES, ... ////////////////////////

	GLuint VAO_cube = vaos.createCubeVAO();
	GLuint VAO_rectangle = vaos.createRectVAO();
	GLuint texture = textures.createSkyboxTexture();
	GLuint VAO_bump_map_floor = vaos.BumpMappingVAO(pos1G, pos2G, pos3G, pos4G, nmG, 1.0f);
	GLuint FloorDiffuse = textures.create2DTexture("FloorDiffuse.PNG");
	GLuint FloorNormal = textures.create2DTexture("FloorNormal.png");
	GLuint VAO_bump_map_wall = vaos.BumpMappingVAO(pos1W, pos2W, pos3W, pos4W, nmW, 10.0f);
	GLuint WallDiffuse = textures.create2DTexture("brickDiffuse.png");
	GLuint WallNormal = textures.create2DTexture("brickNormal.png");


	//////////////////////////// CREATION OF THE SHADERS  /////////////////////////////////////

	//Shader simple(
	//	"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/simple.vert",
	//	"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/simple.frag");
	//simple.compile();										// Don't forget to Compile
	//Shader phong_plus_refl(
	//	"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/light.vert", 
	//	"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/light.frag");
	//phong_plus_refl.compile();
	//Shader skyboxShader(
	//	"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/skybox.vert", 
	//	"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/skybox.frag");
	//skyboxShader.compile();
	//Shader particleShader(
	//	"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/fire.vert", 
	//	"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/fire.frag");
	//particleShader.compile();
	//Shader bumpShader(
	//	"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/bump.vert",
	//	"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/bump.frag");
	//bumpShader.compile();
	//Shader refractionShader(
	//	"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/chromaticAberration.vert", 
	//	"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/chromaticAberration.frag");
	//refractionShader.compile();
	//Shader mirrorShader(
	//	"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/2Dtext.vert", 
	//	"C:/Users/solan/Desktop/VUB material/The third year/Virtual Reality/project_bowling/Glitter/Glitter/Shaders/2Dtext.frag");
	//mirrorShader.compile();

	Shader simple("simple.vert", "simple.frag");
	simple.compile();										// Don't forget to Compile
	Shader phong_plus_refl("light.vert", "light.frag");
	phong_plus_refl.compile();
	Shader skyboxShader("skybox.vert", "skybox.frag");
	skyboxShader.compile();
	Shader particleShader("fire.vert", "fire.frag");
	particleShader.compile();
	Shader bumpShader("bump.vert", "bump.frag");
	bumpShader.compile();
	Shader refractionShader("chromaticAberration.vert", "chromaticAberration.frag");
	refractionShader.compile();
	Shader mirrorShader("2Dtext.vert", "2Dtext.frag");
	mirrorShader.compile();


	//////////////////////////// CREATION OF THE FRAMEBUFFER  /////////////////////////////////////

	// Creation of the framebuffer
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Texture
	GLuint textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	// Attach texture to the framebuffer 
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	//// The depth renderbuffer 
	//GLuint rbo;		// use rbo for write only, no read !
	//glGenRenderbuffers(1, &rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// back to default framebuffer



											////////////////////////// WHILE LOOP ///////////////////////////////////////////////


	while (glfwWindowShouldClose(mWindow) == false) {
		
		time = glfwGetTime();
		showFPS();

		// Variation day - night 
		luminosity = sin(time * 0.1) * 2;
		if (luminosity < 0.15) {
			luminosity = 0.15;
		}


		if (throwBall) {
			components.CreateBall(lastX, lastY, firstThrow);
			renderBall = true;

		}

		pointLightPositions[2] = glm::vec3(5 * sin(time), 3, cos(time) + 44);
		translationMatrixLamp3 = glm::translate(glm::mat4(1.f), pointLightPositions[2]);


		/////////////////////// DEFINE MATRICES /////////////////////////////////////////

		//////// Camera matrices

		rot = glm::rotate(glm::rotate(rot, rot_y, { 0, 1, 0 }), rot_x, { 1, 0, 0 });
		rot_x = 0;
		rot_y = 0;	//reset, otherwise it keeps rotating

		Do_Movement();

		if (renderBall) ballPos = components.BallPosition();
		if (ballView) camera.Position = glm::vec3(ballPos[0], ballPos[1], ballPos[2] - 1.3);
		CameraMatrix = rot * camera.GetViewMatrix();
		ProjectionMatrix = glm::perspective(camera.Zoom, (float)mWidth / (float)mHeight, 0.1f, 1000.0f);
		cameraPosition = camera.Position;


		//////// Model matrices

		// Update dynamics

		components.Simulation();

		// Get matrices we neeed to apply to our objects

		components.PinRendering(matPin1, matPin2, matPin3, matPin4, matPin5, matPin6, matPin7, matPin8, matPin9, matPin10);


		///////////// FIRST PASS - DRAW OFF SCREEN (RENDER ON TEXTURE) ///////////////////

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);


		//////// Ball rendering : lighting + skybox reflection

		phong_plus_refl.use();
		phong_plus_refl.setVector3f("pointLights[0].position", pointLightPositions[0]);
		phong_plus_refl.setVector3f("pointLights[0].color", pointLightColors[0]);
		phong_plus_refl.setFloat("pointLights[0].attenuation", pointLightAttenuations[0]);
		phong_plus_refl.setVector3f("pointLights[1].position", pointLightPositions[1]);
		phong_plus_refl.setVector3f("pointLights[1].color", pointLightColors[1]);
		phong_plus_refl.setFloat("pointLights[1].attenuation", pointLightAttenuations[1]);
		phong_plus_refl.setVector3f("pointLights[2].position", pointLightPositions[2]);
		phong_plus_refl.setVector3f("pointLights[2].color", pointLightColors[2]);
		phong_plus_refl.setFloat("pointLights[2].attenuation", pointLightAttenuations[2]);
		phong_plus_refl.setVector3f("pointLights[3].position", pointLightPositions[3]);
		phong_plus_refl.setVector3f("pointLights[3].color", pointLightColors[3]);
		phong_plus_refl.setFloat("pointLights[3].attenuation", pointLightAttenuations[3]);
		phong_plus_refl.setVector3f("pointLights[4].position", pointLightPositions[4]);
		phong_plus_refl.setVector3f("pointLights[4].color", pointLightColors[0]);
		phong_plus_refl.setFloat("pointLights[4].attenuation", pointLightAttenuations[4]);
		phong_plus_refl.setVector3f("pointLights[5].position", pointLightPositions[5]);
		phong_plus_refl.setVector3f("pointLights[5].color", pointLightColors[0]);
		phong_plus_refl.setFloat("pointLights[5].attenuation", pointLightAttenuations[5]);
		phong_plus_refl.setVector3f("camPos", cameraPositionMirror);
		phong_plus_refl.setMatrix4("View", CameraMatrixMirror);
		phong_plus_refl.setMatrix4("Projection", ProjectionMatrixMirror);
		phong_plus_refl.setFloat("luminosity", luminosity);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		phong_plus_refl.setInteger("skybox", 0);
		phong_plus_refl.setFloat("percReflection", 0.3);
		glDepthMask(GL_TRUE);

		// Render ball only if a ball was thrown
		if (renderBall) {
			////////////////***************
			/*myMotionStateBall->m_graphicsWorldTrans.getOpenGLMatrix(matrixBall);*/
			
			//matBall = components.bulletMatToOpenGLMat(matrixBall);
			components.BallRendering(matBall);
			phong_plus_refl.setVector3f("objectColor", ballColor);
			phong_plus_refl.setMatrix4("Model", matBall);
			ball.Draw(phong_plus_refl);
		}
		

		//////// Pins rendering : lighting only (same shader than for ball)

		phong_plus_refl.setVector3f("objectColor", pinColor);
		phong_plus_refl.setFloat("percReflection", 0);

		// Pin 1
		phong_plus_refl.setMatrix4("Model", matPin1);
		pin.Draw(phong_plus_refl);

		// Pin 2
		phong_plus_refl.setMatrix4("Model", matPin2);
		pin.Draw(phong_plus_refl);

		// Pin 3
		phong_plus_refl.setMatrix4("Model", matPin3);
		pin.Draw(phong_plus_refl);

		// Pin 4
		phong_plus_refl.setMatrix4("Model", matPin4);
		pin.Draw(phong_plus_refl);

		// Pin 5
		phong_plus_refl.setMatrix4("Model", matPin5);
		pin.Draw(phong_plus_refl);

		// Pin 6
		phong_plus_refl.setMatrix4("Model", matPin6);
		pin.Draw(phong_plus_refl);

		// Pin 7
		phong_plus_refl.setMatrix4("Model", matPin7);
		pin.Draw(phong_plus_refl);

		// Pin 8
		phong_plus_refl.setMatrix4("Model", matPin8);
		pin.Draw(phong_plus_refl);

		// Pin 9
		phong_plus_refl.setMatrix4("Model", matPin9);
		pin.Draw(phong_plus_refl);

		// Pin 10
		phong_plus_refl.setMatrix4("Model", matPin10);
		pin.Draw(phong_plus_refl);


		//////// Ground rendering : simple shader 

		simple.use();
		simple.setVector4f("myColorAA", groundColor);
		simple.setMatrix4("View", CameraMatrixMirror);
		simple.setMatrix4("Projection", ProjectionMatrixMirror);
		simple.setMatrix4("Model", transMatrixGround * scaleMatrixGround);
		glBindVertexArray(VAO_cube);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);


		//////// Sphere rendering : refraction (with chromatic aberration)

		refractionShader.use();
		refractionShader.setVector3f("camPos", cameraPositionMirror);
		refractionShader.setMatrix4("View", CameraMatrixMirror);
		refractionShader.setMatrix4("Projection", ProjectionMatrixMirror);
		refractionShader.setMatrix4("Model", transMatrixSp * scaleMatrixSp);
		refractionShader.setFloat("luminosity", luminosity);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		refractionShader.setInteger("skybox", 0);
		sphere.Draw(refractionShader);


		//////// Bump mapping on the bowling line 

		bumpShader.use();
		bumpShader.setMatrix4("View", CameraMatrixMirror);
		bumpShader.setMatrix4("Projection", ProjectionMatrixMirror);
		bumpShader.setMatrix4("Model", translationMatrix);
		bumpShader.setVector3f("camPos", cameraPositionMirror);
		bumpShader.setVector3f("pointLights[0].position", pointLightPositions[0]);
		bumpShader.setVector3f("pointLights[0].color", pointLightColors[0]);
		bumpShader.setFloat("pointLights[0].attenuation", pointLightAttenuations[0]);
		bumpShader.setVector3f("pointLights[1].position", pointLightPositions[1]);
		bumpShader.setVector3f("pointLights[1].color", pointLightColors[1]);
		bumpShader.setFloat("pointLights[1].attenuation", pointLightAttenuations[1]);
		bumpShader.setVector3f("pointLights[2].position", pointLightPositions[2]);
		bumpShader.setVector3f("pointLights[2].color", pointLightColors[2]);
		bumpShader.setFloat("pointLights[2].attenuation", pointLightAttenuations[2]);
		bumpShader.setVector3f("pointLights[3].position", pointLightPositions[3]);
		bumpShader.setVector3f("pointLights[3].color", pointLightColors[3]);
		bumpShader.setFloat("pointLights[3].attenuation", pointLightAttenuations[3]);
		bumpShader.setVector3f("pointLights[4].position", pointLightPositions[4]);
		bumpShader.setVector3f("pointLights[4].color", pointLightColors[0]);
		bumpShader.setFloat("pointLights[4].attenuation", pointLightAttenuations[4]);
		bumpShader.setVector3f("pointLights[5].position", pointLightPositions[5]);
		bumpShader.setVector3f("pointLights[5].color", pointLightColors[0]);
		bumpShader.setFloat("pointLights[5].attenuation", pointLightAttenuations[5]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FloorDiffuse);
		bumpShader.setInteger("myTextureDiffuse", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, FloorNormal);
		bumpShader.setInteger("myTextureNormal", 1);
		glBindVertexArray(VAO_bump_map_floor);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);


		//////// Skybox rendering

		glDepthMask(GL_FALSE);
		skyboxShader.use();
		skyboxShader.setMatrix4("View", CameraMatrixMirror);
		skyboxShader.setMatrix4("Projection", ProjectionMatrixMirror);
		skyboxShader.setMatrix4("Rotation", scaleMatrixSkybox);
		skyboxShader.setFloat("luminosity", luminosity);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		skyboxShader.setInteger("skybox", 0);
		glBindVertexArray(VAO_cube);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(VAO_cube);
		glDepthMask(GL_TRUE);	// Reenable depth writing



								//////// Particles rendering

		GLuint nbNew = 3;		// Nb of new particles at each frame
								// Add new particles
		for (GLuint i = 0; i < nbNew; ++i)
		{
			unusedParticle = FirstUnusedParticle();
			newParticle(particles[unusedParticle], glm::vec3(0, 1, -15));
		}

		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		particleShader.use();
		particleShader.setMatrix4("View", CameraMatrixMirror);
		particleShader.setMatrix4("Projection", ProjectionMatrixMirror);
		particleShader.setMatrix4("Model", glm::mat4(1));

		// Update all particles
		for (GLuint i = 0; i < nbParticles; ++i) {
			Particle &p = particles[i];
			p.lifetime -= dt; // reduce life
			if (p.lifetime > 0.0f) {	// particle is alive, thus update
				p.pos -= p.speed * dt;
				p.color.a -= dt;
				particleShader.setVector3f("offset", p.pos);
				particleShader.setVector4f("color", p.color);
				sphere.Draw(particleShader);
			}
		}
		// Don't forget to reset to default blending mode
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);






		///////////// SECOND PASS - DRAW ON SCREEN (DEFAULT FRAMEBUFFER) ///////////////////

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//////// Mirror rendering : use texture generated in first pass

		mirrorShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		mirrorShader.setInteger("textureColorbuffer", 0);
		mirrorShader.setMatrix4("Model", transMirror * rotMirror * scaleMirror);
		mirrorShader.setMatrix4("View", CameraMatrix);
		mirrorShader.setMatrix4("Projection", ProjectionMatrix);
		glBindVertexArray(VAO_rectangle);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);


		//////// Ball rendering : lighting + skybox reflection

		phong_plus_refl.use();
		phong_plus_refl.setVector3f("pointLights[0].position", pointLightPositions[0]);
		phong_plus_refl.setVector3f("pointLights[0].color", pointLightColors[0]);
		phong_plus_refl.setFloat("pointLights[0].attenuation", pointLightAttenuations[0]);
		phong_plus_refl.setVector3f("pointLights[1].position", pointLightPositions[1]);
		phong_plus_refl.setVector3f("pointLights[1].color", pointLightColors[1]);
		phong_plus_refl.setFloat("pointLights[1].attenuation", pointLightAttenuations[1]);
		phong_plus_refl.setVector3f("pointLights[2].position", pointLightPositions[2]);
		phong_plus_refl.setVector3f("pointLights[2].color", pointLightColors[2]);
		phong_plus_refl.setFloat("pointLights[2].attenuation", pointLightAttenuations[2]);
		phong_plus_refl.setVector3f("pointLights[3].position", pointLightPositions[3]);
		phong_plus_refl.setVector3f("pointLights[3].color", pointLightColors[3]);
		phong_plus_refl.setFloat("pointLights[3].attenuation", pointLightAttenuations[3]);
		phong_plus_refl.setVector3f("pointLights[4].position", pointLightPositions[4]);
		phong_plus_refl.setVector3f("pointLights[4].color", pointLightColors[0]);
		phong_plus_refl.setFloat("pointLights[4].attenuation", pointLightAttenuations[4]);
		phong_plus_refl.setVector3f("pointLights[5].position", pointLightPositions[5]);
		phong_plus_refl.setVector3f("pointLights[5].color", pointLightColors[0]);
		phong_plus_refl.setFloat("pointLights[5].attenuation", pointLightAttenuations[5]);
		phong_plus_refl.setVector3f("camPos", cameraPosition);
		phong_plus_refl.setMatrix4("View", CameraMatrix);
		phong_plus_refl.setMatrix4("Projection", ProjectionMatrix);
		phong_plus_refl.setFloat("luminosity", luminosity);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		phong_plus_refl.setInteger("skybox", 0);
		phong_plus_refl.setFloat("percReflection", 0.3);
		glDepthMask(GL_TRUE);

		if (renderBall) {
			/////////////************
			/*myMotionStateBall->m_graphicsWorldTrans.getOpenGLMatrix(matrixBall);*/
			components.BallRendering(matBall);
			phong_plus_refl.setVector3f("objectColor", ballColor);
			phong_plus_refl.setMatrix4("Model", matBall);
			ball.Draw(phong_plus_refl);
		}

		//////// Pins rendering : lighting only (same shader than for ball)

		phong_plus_refl.setVector3f("objectColor", pinColor);
		phong_plus_refl.setFloat("percReflection", 0);
		// Pin 1
		phong_plus_refl.setMatrix4("Model", matPin1);
		pin.Draw(phong_plus_refl);
		// Pin 2
		phong_plus_refl.setMatrix4("Model", matPin2);
		pin.Draw(phong_plus_refl);
		// Pin 3
		phong_plus_refl.setMatrix4("Model", matPin3);
		pin.Draw(phong_plus_refl);
		// Pin 4
		phong_plus_refl.setMatrix4("Model", matPin4);
		pin.Draw(phong_plus_refl);
		// Pin 5
		phong_plus_refl.setMatrix4("Model", matPin5);
		pin.Draw(phong_plus_refl);
		// Pin 6
		phong_plus_refl.setMatrix4("Model", matPin6);
		pin.Draw(phong_plus_refl);
		// Pin 7
		phong_plus_refl.setMatrix4("Model", matPin7);
		pin.Draw(phong_plus_refl);
		// Pin 8
		phong_plus_refl.setMatrix4("Model", matPin8);
		pin.Draw(phong_plus_refl);
		// Pin 9
		phong_plus_refl.setMatrix4("Model", matPin9);
		pin.Draw(phong_plus_refl);
		// Pin 10
		phong_plus_refl.setMatrix4("Model", matPin10);
		pin.Draw(phong_plus_refl);


		//////// Ground rendering : simple shader

		simple.use();
		simple.setVector4f("myColorAA", groundColor);
		simple.setMatrix4("View", CameraMatrix);
		simple.setMatrix4("Projection", ProjectionMatrix);
		simple.setMatrix4("Model", transMatrixGround * scaleMatrixGround);
		glBindVertexArray(VAO_cube);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//////// White lamp on the wall rendering : simple shader (same shader than for ground)

		simple.setVector4f("myColorAA", glm::vec4(pointLightColors[0], 1));
		simple.setMatrix4("Model", translationMatrixLamp);
		glBindVertexArray(VAO_cube);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//////// Blue lamp on the wall rendering : simple shader (same shader than for ground)

		simple.setVector4f("myColorAA", glm::vec4(pointLightColors[3], 1));
		simple.setMatrix4("Model", translationMatrixLamp2);
		glBindVertexArray(VAO_cube);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//////// Green lamp rendering : simple shader (same shader than for ground)

		simple.setVector4f("myColorAA", glm::vec4(pointLightColors[2], 1));
		simple.setMatrix4("Model", translationMatrixLamp3 * scaleLamp3);
		glBindVertexArray(VAO_cube);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);


		//////// Sphere rendering : refraction (with chromatic aberration)

		refractionShader.use();
		refractionShader.setVector3f("camPos", cameraPosition);
		refractionShader.setMatrix4("View", CameraMatrix);
		refractionShader.setMatrix4("Projection", ProjectionMatrix);
		refractionShader.setMatrix4("Model", transMatrixSp * scaleMatrixSp);
		refractionShader.setFloat("luminosity", luminosity);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		refractionShader.setInteger("skybox", 0);
		sphere.Draw(refractionShader);


		//////// Bump mapping on the bowling line

		bumpShader.use();
		bumpShader.setMatrix4("View", CameraMatrix);
		bumpShader.setMatrix4("Projection", ProjectionMatrix);
		bumpShader.setMatrix4("Model", translationMatrix);
		bumpShader.setVector3f("camPos", cameraPosition);
		bumpShader.setVector3f("pointLights[0].position", pointLightPositions[0]);
		bumpShader.setVector3f("pointLights[0].color", pointLightColors[0]);
		bumpShader.setFloat("pointLights[0].attenuation", pointLightAttenuations[0]);
		bumpShader.setVector3f("pointLights[1].position", pointLightPositions[1]);
		bumpShader.setVector3f("pointLights[1].color", pointLightColors[1]);
		bumpShader.setFloat("pointLights[1].attenuation", pointLightAttenuations[1]);
		bumpShader.setVector3f("pointLights[2].position", pointLightPositions[2]);
		bumpShader.setVector3f("pointLights[2].color", pointLightColors[2]);
		bumpShader.setFloat("pointLights[2].attenuation", pointLightAttenuations[2]);
		bumpShader.setVector3f("pointLights[3].position", pointLightPositions[3]);
		bumpShader.setVector3f("pointLights[3].color", pointLightColors[3]);
		bumpShader.setFloat("pointLights[3].attenuation", pointLightAttenuations[3]);
		bumpShader.setVector3f("pointLights[4].position", pointLightPositions[4]);
		bumpShader.setVector3f("pointLights[4].color", pointLightColors[0]);
		bumpShader.setFloat("pointLights[4].attenuation", pointLightAttenuations[4]);
		bumpShader.setVector3f("pointLights[5].position", pointLightPositions[5]);
		bumpShader.setVector3f("pointLights[5].color", pointLightColors[0]);
		bumpShader.setFloat("pointLights[5].attenuation", pointLightAttenuations[5]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FloorDiffuse);
		bumpShader.setInteger("myTextureDiffuse", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, FloorNormal);
		bumpShader.setInteger("myTextureNormal", 1);
		glBindVertexArray(VAO_bump_map_floor);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);


		//////// Bump mapping on the wall

		bumpShader.setMatrix4("Model", glm::mat4(1));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, WallDiffuse);
		bumpShader.setInteger("myTextureDiffuse", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, WallNormal);
		bumpShader.setInteger("myTextureNormal", 1);
		glBindVertexArray(VAO_bump_map_wall);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);


		//////// Skybox rendering

		glDepthMask(GL_FALSE);

		skyboxShader.use();
		skyboxShader.setMatrix4("View", CameraMatrix);
		skyboxShader.setMatrix4("Projection", ProjectionMatrix);
		skyboxShader.setMatrix4("Rotation", scaleMatrixSkybox);
		skyboxShader.setFloat("luminosity", luminosity);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		skyboxShader.setInteger("skybox", 0);
		glBindVertexArray(VAO_cube);
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
		glBindVertexArray(VAO_cube);
		glDepthMask(GL_TRUE);	// Reenable depth writing



								//////// Particles rendering

								// Add new particles
		for (GLuint i = 0; i < nbNew; ++i)
		{
			unusedParticle = FirstUnusedParticle();
			newParticle(particles[unusedParticle], glm::vec3(0, 1, -15));
		}

		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		particleShader.use();
		particleShader.setMatrix4("View", CameraMatrix);
		particleShader.setMatrix4("Projection", ProjectionMatrix);
		particleShader.setMatrix4("Model", glm::mat4(1));

		// Update all particles
		for (GLuint i = 0; i < nbParticles; ++i) {
			Particle &p = particles[i];
			p.lifetime -= dt; // reduce life
			if (p.lifetime > 0.0f) {	// particle is alive, thus update
				p.pos -= p.speed * dt;
				p.color.a -= dt;
				particleShader.setVector3f("offset", p.pos);
				particleShader.setVector4f("color", p.color);
				sphere.Draw(particleShader);
			}
		}
		// Don't forget to reset to default blending mode
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);

		///////////////////////////// END OF RENDERING /////////////////////////////////

		throwBall = false;

		// Flip Buffers and Draw
		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}

	components.CleanUp();

	glDeleteFramebuffers(1, &fbo);
	glDeleteVertexArrays(1, &VAO_cube);
	glDeleteVertexArrays(1, &VAO_rectangle);
	glDeleteVertexArrays(1, &VAO_bump_map_floor);
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &FloorDiffuse);
	glDeleteTextures(1, &FloorNormal);
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

	if ((keys[GLFW_KEY_0] || keys[GLFW_KEY_KP_0])) {
		throwBall = true;
		std::cout << "Ball thrown" << std::endl;
	}

	if (keys[GLFW_KEY_N]) {
		ballView = !ballView;
		if (!ballView) camera.Position = glm::vec3(0.0f, 15.0f, 70.0f);
	}

	float cameraSpeed = 0.05f; // adjust accordingly
	if (keys[GLFW_KEY_W])
		rot_x = cameraSpeed;
	if (keys[GLFW_KEY_S])
		rot_x = -cameraSpeed;
	if (keys[GLFW_KEY_A])
		rot_y = cameraSpeed;
	if (keys[GLFW_KEY_D])
		rot_y = -cameraSpeed;

}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/) {
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


void Do_Movement() {
	if (keys[GLFW_KEY_UP])  camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_DOWN])    camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_LEFT])    camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_RIGHT])    camera.ProcessKeyboard(RIGHT, deltaTime);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	lastX = xpos; lastY = ypos;
}

void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
}



GLuint FirstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < nbParticles; ++i) {
		if (particles[i].lifetime <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (particles[i].lifetime <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}


void newParticle(Particle &particle, glm::vec3 objPos)
{

	GLfloat random = ((rand() % 100) - 50) / 10.0f;
	GLfloat random2 = ((rand() % 100) - 50) / 10.0f;
	GLfloat random3 = ((rand() % 100) - 50) / 10.0f;
	GLfloat randR = rand() / (float)(RAND_MAX);
	GLfloat randG = rand() / (float)(RAND_MAX);
	GLfloat randB = rand() / (float)(RAND_MAX);
	particle.pos = objPos;
	particle.color = glm::vec4(0.95, 0.7*randG, 0.1*randB, 1.0f);
	particle.lifetime = 1.0f;
	particle.speed = glm::vec3(2 * random, -4 * abs(random2), 2 * random3);

}

