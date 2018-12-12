#include "addComponents.hpp"
#include <vector>
#include <btBulletDynamicsCommon.h>
#include "Model.hpp"

btDynamicsWorld* world;
btDispatcher* dispatcher;
btBroadphaseInterface* broadphase;
btConstraintSolver* solver;
btCollisionConfiguration* collisionConfig;
std::vector<btRigidBody*> bodies;


//btRigidBody* AddComponents::addSphere(float rad, float x, float y, float z, float mass)  // if mass is 0, it's a static object, if mass is not 0, it's a dynamic object
btRigidBody* AddComponents::addSphere(float rad, Model model, btMatrix3x3 rotation, btVector3 translation, float mass)  // if mass is 0, it's a static object, if mass is not 0, it's a dynamic object
{
	float x, y, z;
	//btTransform transform;

	// 想法：给model一个初始的model matrix
	btTransform transform = model.Modeltransformation(rotation, translation);  // it contains a quaternion and position
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



void renderSphere(btRigidBody* sphere)
{
	if (sphere->getCollisionShape()->getShapeType() != SPHERE_SHAPE_PROXYTYPE)	//only render, if it's a sphere
		return;
	glColor3f(1, 0, 0);
	btTransform transform;
	sphere->getMotionState()->getWorldTransform(transform);	//get the transform
	float mat[16];
	transform.getOpenGLMatrix(mat);	//OpenGL matrix stores the rotation and orientation
	//mEntities[i]->updatePose(mat);
	/*void Entity::updatePose(glm::mat4 modelMatrix) {
		mModelMatrix = modelMatrix;
		mModelMatrix = glm::scale(mModelMatrix, mScale);
	}*/
	glPushMatrix();
	glMultMatrixf(mat);	//multiplying the current matrix with it moves the object in place
	glPopMatrix();
}


void AddComponents::addGround() {

	//Adding the ground 
	btTransform t;  // it contains a quaternion and position
	t.setIdentity(); //set the position to (0,0,0) and  orientation to (0,0,0,1)
	t.setOrigin(btVector3(0, 0, 0));  // The position that you want the plane
	btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);  // it's looking upward and it lies on the x,z axis
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);  //?? why only 2 parameters
	btRigidBody* body = new btRigidBody(info);
	world->addRigidBody(body);
	bodies.push_back(body);
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
