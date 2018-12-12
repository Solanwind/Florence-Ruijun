#pragma once

// System Headers
#include <glad/glad.h>
#include <btBulletDynamicsCommon.h>
#include "Model.hpp"

class AddComponents {
public:
	//btRigidBody* addSphere(float rad, float x, float y, float z, float mass);
	btRigidBody* addSphere(float rad, Model model, btMatrix3x3 rotation, btVector3 translation, float mass);
	void addGround();
	void init();
	
};

