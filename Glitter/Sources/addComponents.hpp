#pragma once

// System Headers
#include <glad/glad.h>
#include <btBulletDynamicsCommon.h>
#include "Model.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class AddComponents {
public:

	void addGround();
	void addBowlingAlley();
	void addPins();
	void CleanUp();
	void Simulation();
	void CreateBall(GLfloat xLast, GLfloat yLast, bool &firstThrow);
	void BallRendering(glm::mat4 &matBall);
	btVector3 BallPosition();
	btDiscreteDynamicsWorld* init();
	glm::mat4 bulletMatToOpenGLMat(btScalar	bulletMat[16]);
	void PinRendering(glm::mat4 &matPin1, glm::mat4 &matPin2, glm::mat4 &matPin3, glm::mat4 &matPin4, glm::mat4 &matPin5,
		glm::mat4 &matPin6, glm::mat4 &matPin7, glm::mat4 &matPin8, glm::mat4 &matPin9, glm::mat4 &matPin10);
	
};
