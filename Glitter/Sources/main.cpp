// Local Headers
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
void createBall();
GLuint createCubeVAO();
GLuint create2DTexture(char const * path);
GLuint createSkyboxTexture();
GLuint BumpMappingVAO(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4, glm::vec3 nm, float repeatFactor);
GLuint createRectVAO();
GLuint FirstUnusedParticle();
void newParticle(Particle &particle, glm::vec3 objPos);
btTriangleMesh  * ObjToCollisionShape(std::string inputFile);
glm::mat4 bulletMatToOpenGLMat(btScalar	bulletMat[16]);



int unusedParticle = 0;			// No more life
GLuint nbParticles = 300;
std::vector<Particle> particles;
int lastUsedParticle = 0;

float deltaTime = 0.0f;


glm::mat4 rot = glm::mat4(1.0f);	// identity matrix
float rot_x = 0.0f, rot_y = 0.0f;


btDiscreteDynamicsWorld* myWorld;		// Declaration of the world
btTransform myTransformBall;
btDefaultMotionState *myMotionStateBall;
btScalar	matrixBall[16];
btRigidBody *bodyBall;
btCollisionShape* ballShape;
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


	//////////////////////// PHYSICS INITIALIZATIION ///////////////////////////////////

	///collision configuration contains default setup for memory, collision setup
	btDefaultCollisionConfiguration* myCollisionConfiguration = new btDefaultCollisionConfiguration();
	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher*	myDispatcher = new	btCollisionDispatcher(myCollisionConfiguration);
	btBroadphaseInterface*	myBroadphase = new btDbvtBroadphase();
	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver *mySequentialImpulseConstraintSolver = new btSequentialImpulseConstraintSolver;
	// Initialization of the world
	myWorld = new btDiscreteDynamicsWorld(myDispatcher, myBroadphase, mySequentialImpulseConstraintSolver, myCollisionConfiguration);
	// Set gravity
	myWorld->setGravity(btVector3(0, -9.81, 0));


	// Position, orientation.
	btTransform myTransformGround, myTransformGroundSkybox, myTransformGroundSkybox2, myTransformPin1, myTransformPin2, myTransformPin3,
		myTransformPin4, myTransformPin5, myTransformPin6, myTransformPin7, myTransformPin8, myTransformPin9, myTransformPin10;
	// btDefaultMotionState to synchronize transformations
	btDefaultMotionState *myMotionStateGround, *myMotionStateGroundSkybox, *myMotionStateGroundSkybox2, *myMotionStatePin1, *myMotionStatePin2, *myMotionStatePin3,
		*myMotionStatePin4, *myMotionStatePin5, *myMotionStatePin6, *myMotionStatePin7, *myMotionStatePin8, *myMotionStatePin9, *myMotionStatePin10;
	// to get back position and orientation for OpenGL rendering
	btScalar	matrixPin1[16], matrixPin2[16], matrixPin3[16], matrixPin4[16], matrixPin5[16], matrixPin6[16], matrixPin7[16],
		matrixPin8[16], matrixPin9[16], matrixPin10[16];
	// for rigid bodies
	btRigidBody *bodyGround, *bodyGroundSkybox, *bodyGroundSkybox2, *bodyPin1, *bodyPin2, *bodyPin3, *bodyPin4, *bodyPin5, *bodyPin6, *bodyPin7, *bodyPin8, *bodyPin9, *bodyPin10;
	btScalar mass;


	/////////// Static rigid body - the bowling alley

	btCollisionShape* groundShape = new btBoxShape(btVector3(15, 1, 30));
	myTransformGround.setIdentity();
	myTransformGround.setOrigin(btVector3(0, 0, 15));
	myMotionStateGround = new btDefaultMotionState(myTransformGround);
	btVector3 localInertiaGround(0, 0, 0);
	mass = 0;									// mass = 0 -> static
	btRigidBody::btRigidBodyConstructionInfo rbInfoGround(mass, myMotionStateGround, groundShape, localInertiaGround);
	bodyGround = new btRigidBody(rbInfoGround);
	// Add the body to the dynamics world
	myWorld->addRigidBody(bodyGround);



	/////////// Static rigid body - the ground of the skybox

	btCollisionShape* groundShapeSkybox = new btBoxShape(btVector3(100, 0.01, 100));
	myTransformGroundSkybox.setIdentity();
	myTransformGroundSkybox.setOrigin(btVector3(0, -100, 0));
	myMotionStateGroundSkybox = new btDefaultMotionState(myTransformGroundSkybox);
	btVector3 localInertiaGroundSkybox(0, 0, 0);
	mass = 0;									// mass = 0 -> static
	btRigidBody::btRigidBodyConstructionInfo rbInfoGroundSkybox(mass, myMotionStateGroundSkybox, groundShapeSkybox, localInertiaGroundSkybox);
	bodyGroundSkybox = new btRigidBody(rbInfoGroundSkybox);
	// Add the body to the dynamics world
	myWorld->addRigidBody(bodyGroundSkybox);

	btCollisionShape* groundShapeSkybox2 = new btBoxShape(btVector3(0.01, 100, 100));
	myTransformGroundSkybox2.setIdentity();
	myTransformGroundSkybox2.setOrigin(btVector3(20.01, 0, 0));
	myMotionStateGroundSkybox2 = new btDefaultMotionState(myTransformGroundSkybox2);
	btVector3 localInertiaGroundSkybox2(0, 0, 0);
	mass = 0;									// mass = 0 -> static
	btRigidBody::btRigidBodyConstructionInfo rbInfoGroundSkybox2(mass, myMotionStateGroundSkybox2, groundShapeSkybox2, localInertiaGroundSkybox2);
	bodyGroundSkybox2 = new btRigidBody(rbInfoGroundSkybox2);
	// Add the body to the dynamics world
	myWorld->addRigidBody(bodyGroundSkybox2);


	/////////// Next rigid bodies - Pins

	btCollisionShape* shapePin = new btConvexTriangleMeshShape(ObjToCollisionShape("pin3.obj"));//new btBoxShape(btVector3(1, 3 , 1));
	mass = 0.3f;
	btVector3 localInertiaPin;
	shapePin->calculateLocalInertia(mass, localInertiaPin);

	// Pin 1
	myTransformPin1.setIdentity();
	myTransformPin1.setOrigin(btVector3(0, 3, -6));
	myMotionStatePin1 = new btDefaultMotionState(myTransformPin1);
	btRigidBody::btRigidBodyConstructionInfo rbInfoPin1(mass, myMotionStatePin1, shapePin, localInertiaPin);
	bodyPin1 = new btRigidBody(rbInfoPin1);
	myWorld->addRigidBody(bodyPin1);

	// Pin 2
	myTransformPin2.setIdentity();
	myTransformPin2.setOrigin(btVector3(-2, 3, -8));
	myMotionStatePin2 = new btDefaultMotionState(myTransformPin2);
	btRigidBody::btRigidBodyConstructionInfo rbInfoPin2(mass, myMotionStatePin2, shapePin, localInertiaPin);
	bodyPin2 = new btRigidBody(rbInfoPin2);
	myWorld->addRigidBody(bodyPin2);

	// Pin 3
	myTransformPin3.setIdentity();
	myTransformPin3.setOrigin(btVector3(2, 3, -8));
	myMotionStatePin3 = new btDefaultMotionState(myTransformPin3);
	btRigidBody::btRigidBodyConstructionInfo rbInfoPin3(mass, myMotionStatePin3, shapePin, localInertiaPin);
	bodyPin3 = new btRigidBody(rbInfoPin3);
	myWorld->addRigidBody(bodyPin3);

	// Pin 4
	myTransformPin4.setIdentity();
	myTransformPin4.setOrigin(btVector3(4, 3, -10));
	myMotionStatePin4 = new btDefaultMotionState(myTransformPin4);
	btRigidBody::btRigidBodyConstructionInfo rbInfoPin4(mass, myMotionStatePin4, shapePin, localInertiaPin);
	bodyPin4 = new btRigidBody(rbInfoPin4);
	myWorld->addRigidBody(bodyPin4);

	// Pin 5
	myTransformPin5.setIdentity();
	myTransformPin5.setOrigin(btVector3(0, 3, -10));
	myMotionStatePin5 = new btDefaultMotionState(myTransformPin5);
	btRigidBody::btRigidBodyConstructionInfo rbInfoPin5(mass, myMotionStatePin5, shapePin, localInertiaPin);
	bodyPin5 = new btRigidBody(rbInfoPin5);
	myWorld->addRigidBody(bodyPin5);

	// Pin 6
	myTransformPin6.setIdentity();
	myTransformPin6.setOrigin(btVector3(-4, 3, -10));
	myMotionStatePin6 = new btDefaultMotionState(myTransformPin6);
	btRigidBody::btRigidBodyConstructionInfo rbInfoPin6(mass, myMotionStatePin6, shapePin, localInertiaPin);
	bodyPin6 = new btRigidBody(rbInfoPin6);
	myWorld->addRigidBody(bodyPin6);

	// Pin 7
	myTransformPin7.setIdentity();
	myTransformPin7.setOrigin(btVector3(6, 3, -12));
	myMotionStatePin7 = new btDefaultMotionState(myTransformPin7);
	btRigidBody::btRigidBodyConstructionInfo rbInfoPin7(mass, myMotionStatePin7, shapePin, localInertiaPin);
	bodyPin7 = new btRigidBody(rbInfoPin7);
	myWorld->addRigidBody(bodyPin7);

	// Pin 8
	myTransformPin8.setIdentity();
	myTransformPin8.setOrigin(btVector3(2, 3, -12));
	myMotionStatePin8 = new btDefaultMotionState(myTransformPin8);
	btRigidBody::btRigidBodyConstructionInfo rbInfoPin8(mass, myMotionStatePin8, shapePin, localInertiaPin);
	bodyPin8 = new btRigidBody(rbInfoPin8);
	myWorld->addRigidBody(bodyPin8);

	// Pin 9
	myTransformPin9.setIdentity();
	myTransformPin9.setOrigin(btVector3(-2, 3, -12));
	myMotionStatePin9 = new btDefaultMotionState(myTransformPin9);
	btRigidBody::btRigidBodyConstructionInfo rbInfoPin9(mass, myMotionStatePin9, shapePin, localInertiaPin);
	bodyPin9 = new btRigidBody(rbInfoPin9);
	myWorld->addRigidBody(bodyPin9);

	// Pin 10
	myTransformPin10.setIdentity();
	myTransformPin10.setOrigin(btVector3(-6, 3, -12));
	myMotionStatePin10 = new btDefaultMotionState(myTransformPin10);
	btRigidBody::btRigidBodyConstructionInfo rbInfoPin10(mass, myMotionStatePin10, shapePin, localInertiaPin);
	bodyPin10 = new btRigidBody(rbInfoPin10);
	myWorld->addRigidBody(bodyPin10);



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
	btVector3 ballPos = btVector3(0, 3, 44);

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

	GLuint VAO_cube = createCubeVAO();
	GLuint VAO_rectangle = createRectVAO();
	GLuint texture = createSkyboxTexture();
	GLuint VAO_bump_map_floor = BumpMappingVAO(pos1G, pos2G, pos3G, pos4G, nmG, 1.0f);
	GLuint FloorDiffuse = create2DTexture("FloorDiffuse.PNG");
	GLuint FloorNormal = create2DTexture("FloorNormal.png");
	GLuint VAO_bump_map_wall = BumpMappingVAO(pos1W, pos2W, pos3W, pos4W, nmW, 10.0f);
	GLuint WallDiffuse = create2DTexture("brickDiffuse.png");
	GLuint WallNormal = create2DTexture("brickNormal.png");


	//////////////////////////// CREATION OF THE SHADERS  /////////////////////////////////////

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

	// The depth renderbuffer 
	GLuint rbo;		// use rbo for write only, no read !
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

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
			createBall();
			renderBall = true;
		}

		pointLightPositions[2] = glm::vec3(5*sin(time), 3, cos(time) + 44);
		translationMatrixLamp3 = glm::translate(glm::mat4(1.f), pointLightPositions[2]);


		/////////////////////// DEFINE MATRICES /////////////////////////////////////////

		//////// Camera matrices

		rot = glm::rotate(glm::rotate(rot, rot_y, { 0, 1, 0 }), rot_x, { 1, 0, 0 });
		rot_x = 0;
		rot_y = 0;	//reset, otherwise it keeps rotating

		Do_Movement();
		if (renderBall) ballPos = bodyBall->getCenterOfMassPosition();
		if (ballView) camera.Position = glm::vec3(ballPos[0], ballPos[1], ballPos[2] - 1.3);
		CameraMatrix = rot * camera.GetViewMatrix();
		ProjectionMatrix = glm::perspective(camera.Zoom, (float)mWidth / (float)mHeight, 0.1f, 1000.0f);
		cameraPosition = camera.Position;


		//////// Model matrices

		// Update dynamics
		if (myWorld)
		{
			myWorld->stepSimulation(0.1);
		}

		// Get matrices we neeed to apply to our objects
		myMotionStatePin1->m_graphicsWorldTrans.getOpenGLMatrix(matrixPin1);
		myMotionStatePin2->m_graphicsWorldTrans.getOpenGLMatrix(matrixPin2);
		myMotionStatePin3->m_graphicsWorldTrans.getOpenGLMatrix(matrixPin3);
		myMotionStatePin4->m_graphicsWorldTrans.getOpenGLMatrix(matrixPin4);
		myMotionStatePin5->m_graphicsWorldTrans.getOpenGLMatrix(matrixPin5);
		myMotionStatePin6->m_graphicsWorldTrans.getOpenGLMatrix(matrixPin6);
		myMotionStatePin7->m_graphicsWorldTrans.getOpenGLMatrix(matrixPin7);
		myMotionStatePin8->m_graphicsWorldTrans.getOpenGLMatrix(matrixPin8);
		myMotionStatePin9->m_graphicsWorldTrans.getOpenGLMatrix(matrixPin9);
		myMotionStatePin10->m_graphicsWorldTrans.getOpenGLMatrix(matrixPin10);



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
			myMotionStateBall->m_graphicsWorldTrans.getOpenGLMatrix(matrixBall);
			matBall = bulletMatToOpenGLMat(matrixBall);
			phong_plus_refl.setVector3f("objectColor", ballColor);
			phong_plus_refl.setMatrix4("Model", matBall);
			ball.Draw(phong_plus_refl);
		}


		//////// Pins rendering : lighting only (same shader than for ball)

		phong_plus_refl.setVector3f("objectColor", pinColor);
		phong_plus_refl.setFloat("percReflection", 0);

		// Pin 1
		matPin1 = bulletMatToOpenGLMat(matrixPin1);
		phong_plus_refl.setMatrix4("Model", matPin1);
		pin.Draw(phong_plus_refl);

		// Pin 2
		matPin2 = bulletMatToOpenGLMat(matrixPin2);
		phong_plus_refl.setMatrix4("Model", matPin2);
		pin.Draw(phong_plus_refl);

		// Pin 3
		matPin3 = bulletMatToOpenGLMat(matrixPin3);
		phong_plus_refl.setMatrix4("Model", matPin3);
		pin.Draw(phong_plus_refl);

		// Pin 4
		matPin4 = bulletMatToOpenGLMat(matrixPin4);
		phong_plus_refl.setMatrix4("Model", matPin4);
		pin.Draw(phong_plus_refl);

		// Pin 5
		matPin5 = bulletMatToOpenGLMat(matrixPin5);
		phong_plus_refl.setMatrix4("Model", matPin5);
		pin.Draw(phong_plus_refl);

		// Pin 6
		matPin6 = bulletMatToOpenGLMat(matrixPin6);
		phong_plus_refl.setMatrix4("Model", matPin6);
		pin.Draw(phong_plus_refl);

		// Pin 7
		matPin7 = bulletMatToOpenGLMat(matrixPin7);
		phong_plus_refl.setMatrix4("Model", matPin7);
		pin.Draw(phong_plus_refl);

		// Pin 8
		matPin8 = bulletMatToOpenGLMat(matrixPin8);
		phong_plus_refl.setMatrix4("Model", matPin8);
		pin.Draw(phong_plus_refl);

		// Pin 9
		matPin9 = bulletMatToOpenGLMat(matrixPin9);
		phong_plus_refl.setMatrix4("Model", matPin9);
		pin.Draw(phong_plus_refl);

		// Pin 10
		matPin10 = bulletMatToOpenGLMat(matrixPin10);
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
			myMotionStateBall->m_graphicsWorldTrans.getOpenGLMatrix(matrixBall);
			matBall = bulletMatToOpenGLMat(matrixBall);
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


	// Cleanup.
	for (int i = myWorld->getNumCollisionObjects() - 1; i >= 0; --i) {
		btCollisionObject* obj = myWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}
		myWorld->removeCollisionObject(obj);
		delete obj;
	}
	delete ballShape;
	delete groundShape;
	delete groundShapeSkybox;
	delete shapePin;
	delete myWorld;
	delete mySequentialImpulseConstraintSolver;
	delete myCollisionConfiguration;
	delete myDispatcher;
	delete myBroadphase;
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

void createBall() {

	if (!firstThrow)
		myWorld->removeCollisionObject(bodyBall);

	// Creation of the collision shape
	ballShape = new btSphereShape(1.0);

	myTransformBall.setIdentity();
	myTransformBall.setOrigin(btVector3(0, 3, 44)); 

	btVector3 localInertia;
	btScalar mass = 1.5f;		// mass != 0 -> dynamic
	ballShape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	myMotionStateBall = new btDefaultMotionState(myTransformBall);
	btRigidBody::btRigidBodyConstructionInfo rbInfoBall(mass, myMotionStateBall, ballShape, localInertia);
	bodyBall = new btRigidBody(rbInfoBall);

	// add angular damping (to slow down ball without affecting gravity)
	bodyBall->setDamping(0, 0.5);
	//add the body to the dynamics world
	myWorld->addRigidBody(bodyBall);

	// Add force
	bodyBall->activate(true);
	float forceX = (lastX - 400) / 35;
	float forceZ = (lastY - 300) / 10;
	bodyBall->applyCentralImpulse(btVector3(forceX, 2.0f, -abs(forceZ)));
	firstThrow = false;
}



void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	lastX = xpos; lastY = ypos;
}

void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
}


GLuint createCubeVAO() {

	GLfloat vertices_cube[] = {
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,

	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,

	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f

	};


	GLuint VAO_cube, VBO_cube;
	glGenVertexArrays(1, &VAO_cube);	// Create a VAO (pointer)
	glBindVertexArray(VAO_cube);		// Use the VAO

	// VBO, for axis
	//Copy our vertices array in a buffer for OpenGL to use - Creation of VBO
	glGenBuffers(1, &VBO_cube);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_cube), vertices_cube, GL_STATIC_DRAW);
	// How to interpret datas
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Unbind the VAO (say that we don't use it anymore...)
	glBindVertexArray(0);

	return VAO_cube;
}



GLuint create2DTexture(char const * imName) {

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


GLuint createSkyboxTexture() {

	std::vector<std::string> textures = { "posx.jpg","negx.jpg","posy.jpg","negy.jpg" ,"posz.jpg","negz.jpg" }; // 6 images for the 6 faces of the cube
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












GLuint BumpMappingVAO(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4, glm::vec3 nm, float repeatFactor) {


	// texture coordinates
	glm::vec2 uv1(0.0f, 1.0f * repeatFactor);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f * repeatFactor, 0.0f);
	glm::vec2 uv4(1.0f * repeatFactor, 1.0f * repeatFactor);

	// calculate tangent/bitangent vectors of both triangles
	glm::vec3 tangent1, bitangent1;
	glm::vec3 tangent2, bitangent2;
	// triangle 1
	// ----------
	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent1 = glm::normalize(tangent1);

	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent1 = glm::normalize(bitangent1);

	// triangle 2
	// ----------
	edge1 = pos3 - pos1;
	edge2 = pos4 - pos1;
	deltaUV1 = uv3 - uv1;
	deltaUV2 = uv4 - uv1;

	f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent2 = glm::normalize(tangent2);


	bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent2 = glm::normalize(bitangent2);


	float quadVertices[] = {
		// positions            // normal         // texcoords  // tangent                          // bitangent
		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
	};
	// configure plane VAO
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

	// Unbind the VAO (say that we don't use it anymore...)
	glBindVertexArray(0);

	return VAO;
}




GLuint createRectVAO() {


	GLfloat vertices[] = { -0.5f, 0.5f, 0.0f, 1.0f,
							-0.5f, -0.5f, 0.0f, 0.0f,
							0.5f, -0.5f, 1.0f, 0.0f,
							-0.5f, 0.5f, 0.0f, 1.0f,
							0.5f, -0.5f, 1.0f, 0.0f,
							0.5f, 0.5f, 1.0f, 1.0f };


	// VAO contains pointer to VBO and how to interpret it (to not do it everytime we use the VBO)
	// DRAWING IS DONE IN THE LOOP, BUT TRANSFER OF DATA FROM RAM to VRAM (GPU) IS DONE ONLY ONCE
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);	// Create a VAO (pointer)
	glBindVertexArray(VAO);		// Use the VAO


	// VBO, for triangle
	//Copy our vertices array in a buffer for OpenGL to use - Creation of VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	// How to interpret datas, for session 2, ex 1 (vertex + texture)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));


	// Unbind the VAO (say that we don't use it anymore...)
	glBindVertexArray(0);

	return VAO;
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




btTriangleMesh * ObjToCollisionShape(std::string inputFile) {

	std::string line;
	std::ifstream inFile(inputFile);
	std::vector<btVector3> vertices;
	std::vector<btVector3> triangles;

	while (std::getline(inFile, line)) {
		std::istringstream iss(line);

		char c;
		iss >> c;

		if (line[0] == 'v' && line[1] == ' ') {
			btScalar x, y, z;
			iss >> x >> y >> z;
			btVector3 point(x, y, z);
			vertices.push_back(point);
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			std::string s1, s2, s3;
			iss >> s1 >> s2 >> s3;

			s1 = s1.substr(0, s1.find('/'));
			s2 = s2.substr(0, s2.find('/'));
			s3 = s3.substr(0, s3.find('/'));

			btScalar t1, t2, t3;
			t1 = std::stof(s1) - 1;
			t2 = std::stof(s2) - 1;
			t3 = std::stof(s3) - 1;
			btVector3 triangle(t1, t2, t3);

			triangles.push_back(triangle);
		}
	}

	btTriangleMesh *triangleMesh = new btTriangleMesh();
	for (const btVector3 &triangle : triangles) {
		triangleMesh->addTriangle(vertices[triangle.x()], vertices[triangle.y()], vertices[triangle.z()]);
	}

	return triangleMesh;
}


glm::mat4 bulletMatToOpenGLMat(btScalar	bulletMat[16]) {
	glm::mat4 OpenGLMat = glm::mat4(bulletMat[0], bulletMat[1], bulletMat[2], bulletMat[3],  // first column
		bulletMat[4], bulletMat[5], bulletMat[6], bulletMat[7],  // second column
		bulletMat[8], bulletMat[9], bulletMat[10], bulletMat[11],  // third column
		bulletMat[12], bulletMat[13], bulletMat[14], bulletMat[15]); // fourth column);
	return OpenGLMat;
}



