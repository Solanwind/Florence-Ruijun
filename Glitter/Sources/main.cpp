// Local Headers
// To use stb_image, add this in *one* C++ source file.
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
#include "Camera.hpp"
#include "addComponents.hpp"

#include "btBulletDynamicsCommon.h"
#include <stdio.h>






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
static void mouse_button_callback(GLFWwindow* /*window*/, int button, int action, int /*mods*/);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset);

void showFPS(void);

GLuint createTriangleVAO();
GLuint createAxisVAO();
GLuint createCubeVAO();
GLuint create2DTexture(char const * path);
GLuint createSkyboxTexture();
GLuint BumpMappingVAO();
GLuint BezierVAO();
GLuint createRectVAO();
GLuint FirstUnusedParticle();
void newParticle(Particle &particle, glm::vec3 objPos);
bool DetectCollision(btDiscreteDynamicsWorld* myWorld);
btTriangleMesh  * ObjToCollisionShape(std::string inputFile);
glm::mat4 bulletMatToOpenGLMat(btScalar	bulletMat[16]);

Camera cam;
AddComponents components;
btRigidBody* shooterball;
glm::mat4 combinationmatrix;

int unusedParticle = 0;			// No more life
GLuint nbParticles = 300;
std::vector<Particle> particles;
int lastUsedParticle = 0;

float deltaTime = 0.0f;

float fov = 45.0f;
double xpos_prev;
double ypos_prev;

glm::mat4 rotationMatrix = glm::mat4(1.0f);	// identity matrix
glm::mat4 rot = glm::mat4(1.0f);	// identity matrix
glm::mat4 translationMatrixPin1 = glm::mat4(1.0f);	// identity matrix
float rot_x = 0.0f, rot_y = 0.0f;


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





	///////////////// CAMERA MATRIX //////////////////////////////

	glm::vec3 cameraPosition = glm::vec3(0.0f, 5.0f, 20.0f);	// position of your camera, in world space
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);		// where you want to look at, in world space
	glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);			// probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too

	glm::mat4 CameraMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);	// CameraMatrix = view matrix




																					////////////////// PARTICLES //////////////////////////////////////////

	for (GLuint i = 0; i < nbParticles; ++i) {
		particles.push_back(Particle());
	}
	float dt = 0.01;


	///////////////////////// LOADING MODELS ///////////////////////////////////////////
	Model ball("bowlingBall.obj");
	Model part("sphere.obj");
	Model pin("pin3.obj");


	//////////////////////// PHYSICS ENGINE INITIALIZATIION ///////////////////////////////////


	/*btDiscreteDynamicsWorld* myWorld;		// Declaration of the world
	btBroadphaseInterface*	myBroadphase;
	btCollisionDispatcher*	myDispatcher;
	btDefaultCollisionConfiguration* myCollisionConfiguration;
	btSequentialImpulseConstraintSolver *mySequentialImpulseConstraintSolver;*/


	// Position, orientation.
	btTransform myTransformBall, myTransformLine, myTransformGutter, myTransformPin1, myTransformPin2, myTransformPin3,
		myTransformPin4, myTransformPin5, myTransformPin6, myTransformPin7, myTransformPin8, myTransformPin9, myTransformPin10;
	// btDefaultMotionState to synchronize transformations
	btDefaultMotionState *myMotionStateBall, *myMotionStateGround, *myMotionStatePin1, *myMotionStatePin2, *myMotionStatePin3,
		*myMotionStatePin4, *myMotionStatePin5, *myMotionStatePin6, *myMotionStatePin7, *myMotionStatePin8, *myMotionStatePin9, *myMotionStatePin10;
	// to get back position and orientation for OpenGL rendering
	btScalar	matrixBall[16], matrixPin1[16], matrixPin2[16], matrixPin3[16], matrixPin4[16], matrixPin5[16], matrixPin6[16], matrixPin7[16],
		matrixPin8[16], matrixPin9[16], matrixPin10[16];

	// for rigid bodies
	btRigidBody *bodyBall, *bodyGround, *bodyPin1, *bodyPin2, *bodyPin3, *bodyPin4, *bodyPin5, *bodyPin6, *bodyPin7, *bodyPin8, *bodyPin9, *bodyPin10;

	

	btDynamicsWorld* myWorld = components.init();
	


	/////////// First rigid body - the bowling ball

	// Creation of the collision shape
	//btCollisionShape* ballShape = new btSphereShape(1.0);//new btBoxShape(btVector3(1, 1, 1));

	//myTransformBall.setIdentity();
	//myTransformBall.setOrigin(btVector3(0, 2, 3));

	
	//btVector3 localInertia;
	btScalar mass = 1.5f;		// mass != 0 -> dynamic
	//ballShape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	//myMotionStateBall = new btDefaultMotionState(myTransformBall);
	//btRigidBody::btRigidBodyConstructionInfo rbInfoBall(mass, myMotionStateBall, ballShape, localInertia);
	//bodyBall = new btRigidBody(rbInfoBall);

	//add the body to the dynamics world
	//myWorld->addRigidBody(bodyBall);
	bodyBall = components.addSphere(1.0, 0, 2, 3, 7.26);

	/////////// Second rigid body - the ground

	btCompoundShape* groundShape = new btCompoundShape();
	btCollisionShape* lineShape = new btBoxShape(btVector3(15, 1, 15));
	btCollisionShape* gutterShape = new btBoxShape(btVector3(15, 0.05, 15));

	myTransformLine.setIdentity();
	myTransformLine.setOrigin(btVector3(0, 0, 0));
	groundShape->addChildShape(myTransformLine, lineShape);

	myTransformGutter.setIdentity();
	myTransformGutter.setOrigin(btVector3(0, 0, 0));
	groundShape->addChildShape(myTransformGutter, gutterShape);

	myMotionStateGround = new btDefaultMotionState(myTransformLine, myTransformGutter);


	btVector3 localInertiaGround(0, 0, 0);
	mass = 0;									// mass = 0 -> static

	btRigidBody::btRigidBodyConstructionInfo rbInfoGround(mass, myMotionStateGround, groundShape, localInertiaGround);
	bodyGround = new btRigidBody(rbInfoGround);

	// Add the body to the dynamics world
	myWorld->addRigidBody(bodyGround);


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


	/////////// Apply force on the ball

	bodyBall->activate(true);
	bodyBall->applyCentralImpulse(btVector3(+3.0f, 0.0f, -20.0f));
	//body->setAngularVelocity(btVector3(1, 0, 0));


	//////////////// Parameters for refraction /////////////////////////////////////////
	float nL = 1.0f;  // refractive index of incident medium = air
	float nT = 1.4f;   // refractive index of transmitting medium (the model in our skybox)
	float nLovernT = nL / nT;	// avoid calculation in shaders !
	float nLovernTSquared = pow(nLovernT, 2);	// avoid calculation in shaders !  


	glm::vec3 lightPos = cameraPosition;//glm::vec3(0.0f, 5.0f, 10.0f);
	glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 ballColor = glm::vec3(0.0, 0.5, 0.0);
	glm::vec3 pinColor = glm::vec3(1.0, 1.0, 1.0);

	////////////////////////////// CREATE NEEDED VAOS, TEXTURES, ... ////////////////////////
	GLuint VAO_cube = createCubeVAO();
	GLuint VAO_rectangle = createRectVAO();
	GLuint VAO_triangle = createTriangleVAO();
	GLuint texture = createSkyboxTexture();
	GLuint VAO_bump_map = BumpMappingVAO();
	GLuint texture_diffuse = create2DTexture("FloorDiffuse.PNG");
	GLuint texture_normal = create2DTexture("FloorNormal.png");
	GLuint VAO_bezier = BezierVAO();
	GLuint bowtext = create2DTexture("bow.png");


	//////////////////////////// CREATION OF THE SHADERS  /////////////////////////////////////

	Shader s("simple.vert", 
		"simple.frag");
	s.compile();										// Don't forget to Compile
	Shader smov("light.vert", 
		"light.frag");
	smov.compile();
	Shader skyboxShader("skybox.vert", 
		"skybox.frag");
	skyboxShader.compile();
	Shader s3("fire.vert", 
		"fire.frag");
	s3.compile();
	Shader s2("bump.vert",
		"bump.frag");
	s2.compile();



	///////////////////////// ENABLE DEPTH TEST //////////////////////////////////////////
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	//glDepthFunc(GL_LESS);


	////////////////////////// WHILE LOOP ///////////////////////////////////////////////

	while (glfwWindowShouldClose(mWindow) == false) {
		showFPS();

		// Background Fill Color
		glClearColor(0.00f, 1.00f, 1.00f, 1.0f);
		// Clear color buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDepthMask(GL_FALSE);		// Remove depth writing


									/////////////////////// DEFINE MATRICES /////////////////////////////////////////

		glm::mat4 CameraMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
		glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.5f, 100.0f);
		rotationMatrix = glm::rotate(glm::rotate(rotationMatrix, rot_y, { 0, 1, 0 }), rot_x, { 1, 0, 0 });
		rot_x = 0;
		rot_y = 0;	//reset, otherwise it keeps rotating

		double time = glfwGetTime();
		//CameraMatrix = glm::lookAt(glm::vec3(cos(time), 0.0, sin(time)), cameraTarget, upVector);			//Session 2, "make it spin"



		///////////////// SKYBOX ////////////////////


		glDepthMask(GL_FALSE);

		skyboxShader.use();							// Use shader

		glm::mat4 scaleMatrixSkybox = glm::scale(glm::mat4(1.f), glm::vec3(50, 50, 50));
		glm::mat4 translationMatrixSkybox = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0));
		skyboxShader.setMatrix4("View", CameraMatrix);				// "give info to shaders"
		skyboxShader.setMatrix4("Projection", ProjectionMatrix);
		skyboxShader.setMatrix4("Rotation", rotationMatrix * translationMatrixSkybox * scaleMatrixSkybox);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		skyboxShader.setInteger("skybox", 0);
		glBindVertexArray(VAO_cube);
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
		glBindVertexArray(VAO_cube);
		glDepthMask(GL_TRUE);	// Reenable depth writing


								// Update dynamics
		if (myWorld)
		{
			//printf("%d", DetectCollision(myWorld));
			myWorld->stepSimulation(0.01);
		}

		// We get the matrices we neeed to apply to our objects
		
	//	myMotionStateBall->m_graphicsWorldTrans.getOpenGLMatrix(matrixBall);
		glm::mat4 matBall = components.renderSphere(bodyBall);
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

		// We render the objects with the transformations applied to them

		/////////// Ball rendering

		smov.use();							// Use shader

		glm::mat4 scaleMatrixBall = glm::scale(glm::mat4(1.f), glm::vec3(1, 1, 1));

		/*glm::mat4 matBall = glm::mat4(matrixBall[0], matrixBall[1], matrixBall[2], matrixBall[3],  // first column
			matrixBall[4], matrixBall[5], matrixBall[6], matrixBall[7],  // second column
			matrixBall[8], matrixBall[9], matrixBall[10], matrixBall[11],  // third column
			matrixBall[12], matrixBall[13], matrixBall[14], matrixBall[15]); // fourth column);*/



		smov.setVector3f("lightColor", lightColor);
		smov.setVector3f("lightPos", lightPos);
		smov.setVector3f("objectColor", ballColor);
		smov.setVector3f("camPos", cameraPosition);
		smov.setMatrix4("View", CameraMatrix);
		smov.setMatrix4("Projection", ProjectionMatrix);
		smov.setMatrix4("Model", rotationMatrix * matBall * scaleMatrixBall);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		smov.setInteger("skybox", 0);
		smov.setFloat("percReflection", 0.2);
		glDepthMask(GL_TRUE);
		ball.Draw(smov);



		if (keys[GLFW_KEY_SPACE]) {
			shooterball = components.addSphere(1.0, 0, 2, 3, 7.26);
			shooterball->activate(true);
			shooterball->applyCentralImpulse(btVector3(+0.0f, 0.0f, -80.0f));
			combinationmatrix = components.renderSphere(shooterball);  // it needs to be updated every loop
			cout << "You have pressed the shooting button!" << endl;
		}
	

		if (combinationmatrix[0][0] != 0) {
			combinationmatrix = components.renderSphere(shooterball);
		}
		else {
			combinationmatrix;
		}
		

		smov.setVector3f("lightColor", lightColor);
		smov.setVector3f("lightPos", lightPos);
		smov.setVector3f("objectColor", ballColor);
		smov.setVector3f("camPos", cameraPosition);
		smov.setMatrix4("View", CameraMatrix);
		smov.setMatrix4("Projection", ProjectionMatrix);
		smov.setMatrix4("Model", rotationMatrix * combinationmatrix * scaleMatrixBall);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		smov.setInteger("skybox", 0);
		smov.setFloat("percReflection", 0.2);
		glDepthMask(GL_TRUE);
		ball.Draw(smov);


		/////////// Pins rendering

		smov.setVector3f("objectColor", pinColor);
		smov.setFloat("percReflection", 0);

		// Pin 1
		glm::mat4 matPin1 = bulletMatToOpenGLMat(matrixPin1);
		smov.setMatrix4("Model", rotationMatrix * matPin1);
		pin.Draw(smov);

		// Pin 2
		glm::mat4 matPin2 = bulletMatToOpenGLMat(matrixPin2);
		smov.setMatrix4("Model", rotationMatrix * matPin2);
		pin.Draw(smov);

		// Pin 3
		glm::mat4 matPin3 = bulletMatToOpenGLMat(matrixPin3);
		smov.setMatrix4("Model", rotationMatrix * matPin3);
		pin.Draw(smov);

		// Pin 4
		glm::mat4 matPin4 = bulletMatToOpenGLMat(matrixPin4);
		smov.setMatrix4("Model", rotationMatrix * matPin4);
		pin.Draw(smov);

		// Pin 5
		glm::mat4 matPin5 = bulletMatToOpenGLMat(matrixPin5);
		smov.setMatrix4("Model", rotationMatrix * matPin5);
		pin.Draw(smov);

		// Pin 6
		glm::mat4 matPin6 = bulletMatToOpenGLMat(matrixPin6);
		smov.setMatrix4("Model", rotationMatrix * matPin6);
		pin.Draw(smov);

		// Pin 7
		glm::mat4 matPin7 = bulletMatToOpenGLMat(matrixPin7);
		smov.setMatrix4("Model", rotationMatrix * matPin7);
		pin.Draw(smov);

		// Pin 8
		glm::mat4 matPin8 = bulletMatToOpenGLMat(matrixPin8);
		smov.setMatrix4("Model", rotationMatrix * matPin8);
		pin.Draw(smov);

		// Pin 9
		glm::mat4 matPin9 = bulletMatToOpenGLMat(matrixPin9);
		smov.setMatrix4("Rotation", rotationMatrix * matPin9);
		pin.Draw(smov);

		// Pin 10
		glm::mat4 matPin10 = bulletMatToOpenGLMat(matrixPin10);
		smov.setMatrix4("Model", rotationMatrix * matPin10);
		pin.Draw(smov);



		/////////// Ground rendering

		s.use();							// Use shader
		s.setVector4f("myColorAA", glm::vec4(1.0, 0.7, 0.4, 1.0));

		glm::mat4 scaleMatrixGround = glm::scale(glm::mat4(1.f), glm::vec3(15, 1, 15));

		s.setMatrix4("View", CameraMatrix);
		s.setMatrix4("Projection", ProjectionMatrix);
		s.setMatrix4("Rotation", rotationMatrix * scaleMatrixGround);

		glBindVertexArray(VAO_cube);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);




		///////////////////////// PARTICLES /////////////////////////////////////
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

		s3.use();
		s3.setMatrix4("View", CameraMatrix);
		s3.setMatrix4("Projection", ProjectionMatrix);
		s3.setMatrix4("Rotation", rotationMatrix);

		// Update all particles
		for (GLuint i = 0; i < nbParticles; ++i) {
			Particle &p = particles[i];
			p.lifetime -= dt; // reduce life
			if (p.lifetime > 0.0f) {	// particle is alive, thus update
				p.pos -= p.speed * dt;
				p.color.a -= dt;
				s3.setVector3f("offset", p.pos);
				s3.setVector4f("color", p.color);
				part.Draw(s3);
			}
		}
		// Don't forget to reset to default blending mode
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);






		//////////////////// THIRD SHADER ///////////////////////////////////


		// bump mapping
		s2.use();							// Use shader

		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1, 1, 1));
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0, 1.01, 0));
		s2.setMatrix4("View", CameraMatrix);
		s2.setMatrix4("Projection", ProjectionMatrix);
		s2.setMatrix4("Rotation", rotationMatrix * translationMatrix * scaleMatrix);
		s2.setVector3f("camPos", cameraPosition);
		s2.setVector3f("lightColor", lightColor);
		s2.setVector3f("lightPos", lightPos);

		glDepthMask(GL_TRUE);	// Reenable depth writing

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_diffuse);
		s2.setInteger("myTextureDiffuse", 0); // glUniform1i(glGetUniformLocation(shaderProgramID, textureNameInFragmentShader"), 0);


		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_normal);
		s2.setInteger("myTextureNormal", 1);

		glBindVertexArray(VAO_bump_map);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);







		/*			// BEZIER
		s3.use();							// Use shader
		translationMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0));
		s3.setMatrix4("View", CameraMatrix);
		s3.setMatrix4("Projection", ProjectionMatrix);
		s3.setMatrix4("Rotation", rotationMatrix * translationMatrix);
		s3.setVector3f("controlPoint1", glm::vec3(-0.5,-0.5,0.0));
		s3.setVector3f("controlPoint2", glm::vec3(sin(time), cos(time), sin(time)));
		s3.setVector3f("controlPoint3", glm::vec3(cos(time), sin(time), cos(time)));
		s3.setVector3f("controlPoint4", glm::vec3(0.5, 0.5, 0.0));

		glDepthMask(GL_TRUE);	// Reenable depth writing
		glBindVertexArray(VAO_bezier);
		glDrawArrays(GL_LINE_STRIP, 0, 21);  //21 = size of t
		glBindVertexArray(0);
		*/

		/*
		s3.use();							// Use shader
		s3.setMatrix4("View", CameraMatrix);
		s3.setMatrix4("Projection", ProjectionMatrix);
		s3.setMatrix4("Rotation", rotationMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bowtext);
		s3.setInteger("myTexture1", 0); // glUniform1i(glGetUniformLocation(shaderProgramID, textureNameInFragmentShader"), 0);
		glBindVertexArray(VAO_rectangle);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		*/






		//////////////////////// SOME OTHER CODE ////////////////////////////////

		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		s.setInteger("myTexture1", 0); // glUniform1i(glGetUniformLocation(shaderProgramID, textureNameInFragmentShader"), 0);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		//glBindVertexArray(VAO_axis);
		//glDrawArrays(GL_LINES, 0, 6);
		//glBindVertexArray(0);
		//glBindVertexArray(VAO_cube);
		//glDrawArrays(GL_TRIANGLES, 0, 12*3);
		//glBindVertexArray(0);
		*/



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
		std::cout << "You have pressed 0" << std::endl;
	}

	// https://learnopengl.com/Getting-started/Camera // To move 
	float cameraSpeed = 0.05f; // adjust accordingly

	if (keys[GLFW_KEY_UP]) {
		rot_x = cameraSpeed;
	}
	if (keys[GLFW_KEY_DOWN]) {
		rot_x = -cameraSpeed;
	}
	if (keys[GLFW_KEY_LEFT]) {
		rot_y = cameraSpeed;
	}

	if (keys[GLFW_KEY_RIGHT]) {
		rot_y = -cameraSpeed;
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
	/*
	if (keys[GLFW_MOUSE_BUTTON_RIGHT]) {
	std::cout << "Mouse Position : (" << xpos << ", " << ypos << ")" << std::endl;
	}
	*/
	float cameraSpeed = 0.05f; // adjust accordingly
	if (keys[GLFW_MOUSE_BUTTON_LEFT]) {
		if (xpos_prev - xpos > 0) {
			rot_y = -cameraSpeed;
		}
		else if (xpos_prev - xpos < 0) {
			rot_y = +cameraSpeed;
		}
		if (ypos_prev - ypos > 0) {
			rot_x = -cameraSpeed;
		}
		else if (ypos_prev - ypos < 0) {
			rot_x = +cameraSpeed;
		}
	}
	xpos_prev = xpos;
	ypos_prev = ypos;
}

void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset)
{
	/*
	if (keys[GLFW_MOUSE_BUTTON_LEFT]) {
	std::cout << "Mouse Offset : " << yoffset << std::endl;
	}
	*/
	/*
	float cameraSpeed = 0.05f; // adjust accordingly
	cameraFront = cameraPosition - cameraTarget;
	if (yoffset < 0) {
	cameraPosition += cameraSpeed * cameraFront;
	}
	if (yoffset > 0) {
	cameraPosition -= cameraSpeed * cameraFront;
	}
	*/

	if (fov < 10.0f) {
		fov = 10.0f;
	}
	else if (fov <= 90.0f) {
		fov -= yoffset;
	}
	else {
		fov = 90.0f;
	}

}





GLuint createTriangleVAO() {

	GLfloat vertices[] = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f };      //ex7

																						   /*		Session 1, question 8
																						   GLfloat vertices[] = { -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
																						   0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
																						   0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f };
																						   // vertex(3 coord + RGBA)
																						   */

																						   /*
																						   GLfloat vertices[] = { -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,						//coord of the vertex + coord of the texturing im (in uv space)
																						   0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
																						   0.0f, 0.5f, 0.0f, 0.5f, 1.0f };						//session2,ex1
																						   */

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


	// How to interpret datas, for session 1, ex 7 (only vertex)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);


	/*
	// How to interpret datas, for session 1, ex 8  (vertex + color)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	*/

	/*
	// How to interpret datas, for session 2, ex 1 (vertex + texture)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	*/

	// Unbind the VAO (say that we don't use it anymore...)
	glBindVertexArray(0);

	return VAO;
}



GLuint createAxisVAO() {

	// Axis: 2 vertices per line
	GLfloat vertices_axis[] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,				//origin + 1,0,0 (x axis)
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

	// GOOD PRACTICE: 1 VBO PER VAO (otherwise, difficult)
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




GLuint createCubeVAO() {


	// Vertex only
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
	/*
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	*/

	// Unbind the VAO (say that we don't use it anymore...)
	glBindVertexArray(0);

	////////////////////// END OF VAO FOR CUBE ////////////////////////////////

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
	//std::vector<std::string> textures = { "posx.jpg","negx.jpg","posy.jpg","negy.jpg" ,"testPosz.jpg","negz.jpg" };
	std::vector<std::string> textures = { 
		"posx.jpg",
		"negx.jpg",
		"posy.jpg",
		"negy.jpg" ,
		"posz.jpg",
		"negz.jpg" }; // 6 images for the 6 faces of the cube
																												//std::vector<std::string> textures = { "checkerboard.jpg","checkerboard.jpg","checkerboard.jpg","checkerboard.jpg" ,"checkerboard.jpg","checkerboard.jpg" };
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












GLuint BumpMappingVAO() {

	GLfloat vertices[] = { -15.0f, 0.0f, -15.0f,
		15.0f, 0.0f, -15.0f,
		-15.0f, 0.0f, 15.0f,
		15.0f, 0.0f, -15.0f,
		-15.0f, 0.0f, 15.0f,
		15.0f, 0.0f, 15.0f };

	GLfloat uvs[] = { 0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f };


	GLfloat tangents[std::size(vertices) / 3] = {};
	GLfloat bitangents[std::size(vertices) / 3] = {};

	int j = 0;
	for (int i = 0; i < std::size(vertices); i += 9) {		// for each triangle

		glm::vec3 pos1(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
		glm::vec3 pos2(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
		glm::vec3 pos3(vertices[i + 6], vertices[i + 7], vertices[i + 8]);

		glm::vec2 uv1(uvs[i + 0 - j * 3], uvs[i + 1 - j * 3]);
		glm::vec2 uv2(uvs[i + 2 - j * 3], uvs[i + 3 - j * 3]);
		glm::vec2 uv3(uvs[i + 4 - j * 3], uvs[i + 5 - j * 3]);
		j += 1;

		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		glm::vec3 tangent = (edge1 * deltaUV2.y - edge2 * deltaUV1.y) * f;
		glm::vec3 bitangent = (edge2 * deltaUV1.x - edge1 * deltaUV2.x) * f;

		tangents[i / 3] = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangents[i / 3 + 1] = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangents[i / 3 + 2] = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		bitangents[i / 3] = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangents[i / 3 + 1] = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangents[i / 3 + 2] = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

	}


	// VAO contains pointer to VBO and how to interpret it (to not do it everytime we use the VBO)
	// DRAWING IS DONE IN THE LOOP, BUT TRANSFER OF DATA FROM RAM to VRAM (GPU) IS DONE ONLY ONCE
	GLuint VAO;
	glGenVertexArrays(1, &VAO);	// Create a VAO (pointer)
	glBindVertexArray(VAO);		// Use the VAO

	GLuint verticesBuffer;
	glGenBuffers(1, &verticesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint uvsBuffer;
	glGenBuffers(1, &uvsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);

	GLuint tangentsBuffer;
	glGenBuffers(1, &tangentsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, tangentsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tangents), tangents, GL_STATIC_DRAW);

	GLuint bitangentsBuffer;
	glGenBuffers(1, &bitangentsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bitangents), bitangents, GL_STATIC_DRAW);



	// How to interpret datas

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvsBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

	// 3rd attribute buffer : tangents
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, tangentsBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// 4th attribute buffer : bitangents
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentsBuffer);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);



	// Unbind the VAO (say that we don't use it anymore...)
	glBindVertexArray(0);

	return VAO;
}



GLuint BezierVAO() {

	GLfloat t[] = { 0.0,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95,1.0 };

	GLuint VAO_bezier, VBO_bezier;
	glGenVertexArrays(1, &VAO_bezier);	// Create a VAO (pointer)
	glBindVertexArray(VAO_bezier);		// Use the VAO

										// VBO, for axis
										//Copy our vertices array in a buffer for OpenGL to use - Creation of VBO
	glGenBuffers(1, &VBO_bezier);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_bezier);
	glBufferData(GL_ARRAY_BUFFER, sizeof(t), t, GL_STATIC_DRAW);
	// How to interpret datas
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(GLfloat), (GLvoid*)0);

	// Unbind the VAO (say that we don't use it anymore...)
	glBindVertexArray(0);

	return VAO_bezier;
}


GLuint createRectVAO() {


	GLfloat vertices[] = { -1.0f, 0.0f, -3.0f, 0.0f, 0.0f,
		1.0f, 0.0f, -3.0f, 1.0f, 0.0f,
		-1.0f, 0.0f, 3.0f, 0.0f, 1.0f,
		1.0f, 0.0f, -3.0f, 1.0f, 0.0f,
		-1.0f, 0.0f, 3.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 3.0f, 1.0f, 1.0f };


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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));


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
	particle.pos = objPos; // glm::vec3(((rand() % 100) - 50) / 500.0f, ((rand() % 100) - 50) / 500.0f, ((rand() % 100) - 50) / 500.0f);// +random;
	particle.color = glm::vec4(0.95, 0.7*randG, 0.1*randB, 1.0f);
	particle.lifetime = 1.0f;
	particle.speed = glm::vec3(2 * random, -4 * abs(random2), 2 * random3);

}


bool DetectCollision(btDiscreteDynamicsWorld* myWorld) {
	bool IsCollision = false;
	int numManifolds = myWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = myWorld->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		if (!obA->getCollisionShape()->isNonMoving() && !obB->getCollisionShape()->isNonMoving()) {
			IsCollision = true;
		}
	}

	return IsCollision;

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
