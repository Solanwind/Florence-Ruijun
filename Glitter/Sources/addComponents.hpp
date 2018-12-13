#pragma once

// System Headers
#include <glad/glad.h>
#include <btBulletDynamicsCommon.h>
#include "Model.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class AddComponents {
public:
	//btRigidBody* addSphere(float rad, float x, float y, float z, float mass);
	btRigidBody* AddComponents::addSphere(float rad, float x, float y, float z, float mass);
	glm::mat4 renderSphere(btRigidBody* sphere);
	void addGround();
	btDynamicsWorld* AddComponents::init();
	void simulation();
	
};
