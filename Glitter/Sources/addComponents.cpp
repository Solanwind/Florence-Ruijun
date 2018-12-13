#include "addComponents.hpp"
#include <vector>
#include <btBulletDynamicsCommon.h>
#include "Model.hpp"
#include <glad/glad.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

btDynamicsWorld* world;
btDispatcher* dispatcher;
btBroadphaseInterface* broadphase;
btConstraintSolver* solver;
btCollisionConfiguration* collisionConfig;
std::vector<btRigidBody*> bodies;


//btRigidBody* AddComponents::addSphere(float rad, float x, float y, float z, float mass)  // if mass is 0, it's a static object, if mass is not 0, it's a dynamic object
btRigidBody* AddComponents::addSphere(float rad, float x, float y, float z,  float mass)  // if mass is 0, it's a static object, if mass is not 0, it's a dynamic object
{
	
	btTransform transform;
	// 想法：给model一个初始的model matrix
	//btTransform transform = model.Modeltransformation(rotation, translation);  // Give the model's rotation and translation to the transform
	transform.setIdentity(); //set the position to (0,0,0) and  orientation to (0,0,0,1)
	transform.setOrigin(btVector3(x, y, z));  //put it to x,y,z coordinates
	btSphereShape* sphere = new btSphereShape(rad);  // rad =  0.1091565f
	btVector3 inertia(0, 0, 0);  //inertia is 0,0,0 for static object, else
	if (mass != 0.0)
	{
		sphere->calculateLocalInertia(mass, inertia);
	}
	btMotionState* motion = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);  //?? why only 2 parameters
	btRigidBody* body = new btRigidBody(info);

	world->addRigidBody(body);
	bodies.push_back(body);
	
	return body;
}



glm::mat4 AddComponents::renderSphere(btRigidBody* sphere, glm::vec3 mScale)
{
	//if (sphere->getCollisionShape()->getShapeType() != SPHERE_SHAPE_PROXYTYPE)	//only render, if it's a sphere
	//	return ;
	glColor3f(1, 0, 0);
	btTransform transform;
	sphere->getMotionState()->getWorldTransform(transform);	//get the transform
	glm::mat4 modelmatrix;
	transform.getOpenGLMatrix(glm::value_ptr(modelmatrix));	//OpenGL matrix stores the rotation and orientation
	modelmatrix = glm::scale(modelmatrix, mScale);


	return modelmatrix;
}


void AddComponents::addGround() {
	// Infinite plane defined by normal
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

	btDefaultMotionState* groundMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);

	world->addRigidBody(groundRigidBody);
}

void AddComponents::init()
{
	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	broadphase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
	world->setGravity(btVector3(0, -10, 0));
}

void AddComponents::simulation() {
	world->stepSimulation(0.001);
}
