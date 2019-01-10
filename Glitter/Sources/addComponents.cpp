#include "addComponents.hpp"
#include <vector>
#include <btBulletDynamicsCommon.h>
#include <glad/glad.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


////////////////////////////////////////////////////////////////////////////////////

btDiscreteDynamicsWorld* myWorld;		// Declaration of the world
btTransform myTransformBall;
btDefaultMotionState *myMotionStateBall;
btScalar BallMatrix[16];
btRigidBody *bodyBall;
btCollisionShape* ballShape;
GLfloat xLast = 400, yLast = 300;



//////////////////////////////////////////////////////////////////////////////////////////

btDefaultCollisionConfiguration* myCollisionConfiguration;
///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
btCollisionDispatcher*	myDispatcher;
btBroadphaseInterface*	myBroadphase;
///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
btSequentialImpulseConstraintSolver *mySequentialImpulseConstraintSolver;
// Initialization of the world
btTriangleMesh  * ObjToCollisionShape(std::string inputFile);
glm::mat4 bulletMatToOpenGLMat(btScalar	bulletMat[16]);

/////////////////////////////////////////////////////////////////////////////////////
btCollisionShape* shapePin;
btCollisionShape* groundShape;
btCollisionShape* groundShapeSkybox;
btCollisionShape* groundShapeSkybox2;



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


///////////////////////////////////////////////////////////////////////////////////////


void AddComponents::addBowlingAlley()
{
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

}

void AddComponents::addGround() 
{

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

}

void AddComponents::addPins() 
{

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


}


void AddComponents::CreateBall(GLfloat xLast, GLfloat yLast, bool &firstBallThrow) {

	if (!firstBallThrow)
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
	float forceX = (xLast - 400) / 35;
	float forceZ = (yLast - 300) / 10;
	bodyBall->applyCentralImpulse(btVector3(forceX, 2.0f, -abs(forceZ)));
	firstBallThrow = false;

}



btDiscreteDynamicsWorld* AddComponents::init()
{
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
	return myWorld;
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

btVector3 AddComponents::BallPosition() 
{
	btVector3 ballPos = btVector3(0, 3, 44);
	return ballPos = bodyBall->getCenterOfMassPosition();

}

void AddComponents::Simulation() {
	myWorld->stepSimulation(0.001);
}

void AddComponents::BallRendering(glm::mat4 &matBall)
{

	myMotionStateBall->m_graphicsWorldTrans.getOpenGLMatrix(BallMatrix);
	matBall = bulletMatToOpenGLMat(BallMatrix);

}


void AddComponents::PinRendering(glm::mat4 &matPin1, glm::mat4 &matPin2, glm::mat4 &matPin3, glm::mat4 &matPin4, glm::mat4 &matPin5,
	glm::mat4 &matPin6, glm::mat4 &matPin7, glm::mat4 &matPin8, glm::mat4 &matPin9, glm::mat4 &matPin10)
{
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

	matPin1 = bulletMatToOpenGLMat(matrixPin1);
	matPin2 = bulletMatToOpenGLMat(matrixPin2);
	matPin3 = bulletMatToOpenGLMat(matrixPin3);
	matPin4 = bulletMatToOpenGLMat(matrixPin4);
	matPin5 = bulletMatToOpenGLMat(matrixPin5);
	matPin6 = bulletMatToOpenGLMat(matrixPin6);
	matPin7 = bulletMatToOpenGLMat(matrixPin7);
	matPin8 = bulletMatToOpenGLMat(matrixPin8);
	matPin9 = bulletMatToOpenGLMat(matrixPin9);
	matPin10 = bulletMatToOpenGLMat(matrixPin10);

	
}

glm::mat4 AddComponents::bulletMatToOpenGLMat(btScalar bulletMat[16]) {
	glm::mat4 OpenGLMat = glm::mat4(bulletMat[0], bulletMat[1], bulletMat[2], bulletMat[3],  // first column
		bulletMat[4], bulletMat[5], bulletMat[6], bulletMat[7],  // second column
		bulletMat[8], bulletMat[9], bulletMat[10], bulletMat[11],  // third column
		bulletMat[12], bulletMat[13], bulletMat[14], bulletMat[15]); // fourth column);
	return OpenGLMat;
}

void AddComponents::CleanUp() {
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

}

