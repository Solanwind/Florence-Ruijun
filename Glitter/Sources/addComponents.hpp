#pragma once

// System Headers
#include <glad/glad.h>
#include <btBulletDynamicsCommon.h>
#include "Model.hpp"

class AddComponents {
public:
	//btRigidBody* addSphere(float rad, float x, float y, float z, float mass);
	btRigidBody* AddComponents::addSphere(float rad, float x, float y, float z, float mass);
	glm::mat4 renderSphere(btRigidBody* sphere, glm::vec3 mScale);
	void addGround();
	void init();
	void simulation();
	
};

