#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Lemming.h"
#include "Game.h"
#include "ShaderManager.h"
#include "JobFactory.h"
#include "Utils.h"

#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4


void Lemming::init(Job *job, const glm::vec2 &initialPosition)
{
	this->shaderProgram = &ShaderManager::getInstance().getShaderProgram();
	this->job = job;
	this->job->initAnims(*shaderProgram);
	sprite = this->job->getJobSprite();
	sprite->setPosition(initialPosition);
	alive = true;
}

void Lemming::update(int deltaTime)
{
	if (sprite->update(deltaTime) == 0) {
		return;
	}

	if (outOfMap()) {
		alive = false;
		delete this->job;

	}
	else {
		if (countdown != NULL && countdown->isOver()) {
			changeJob(JobFactory::instance().createExploderJob());
			delete countdown;
			countdown = NULL;
		}
		else {
			job->updateStateMachine(deltaTime);

			if (countdown != NULL) {
				countdown->setPosition(glm::vec2(6, -8) + sprite->position());
				countdown->update(deltaTime);
			}

			if (job->finished()) {
				alive = (job->getNextJob() != NULL);
				if (alive) {
					changeJob(job->getNextJob());
				}
			}
		}
	}
	
}

void Lemming::render()
{
	glm::vec2 oldPosition = sprite->position();
	sprite->setPosition(sprite->position() - Level::currentLevel().getLevelAttributes()->cameraPos);
	sprite->render();
	sprite->setPosition(oldPosition);

	if (countdown != NULL) {
		countdown->render();
	}
}

void Lemming::changeJob(Job *nextJob)
{
	walkingRight = job->isWalkingRight();
	delete this->job;
	this->job = nextJob;
	this->job->initAnims(*shaderProgram);
	nextJob->setWalkingRight(walkingRight);

	glm::ivec2 oldPosition = sprite->position();

	delete sprite;
	sprite = this->job->getJobSprite();
	sprite->setPosition(oldPosition);
}

glm::vec2& Lemming::getPosition()
{
	return sprite->position();
}

Job* Lemming::getJob()
{
	return job;
}

bool Lemming::isAlive()
{
	return alive;
}

void Lemming::changeAlive()
{
	alive = !alive;
}

bool Lemming::isWalkingRight() 
{
	return walkingRight;
}

void Lemming::setWalkingRight(bool value)
{
	walkingRight = value;
}

void Lemming::writeDestiny()
{
	countdown = new Countdown();
}

bool Lemming::outOfMap()
{
	return !Utils::insideRectangle(sprite->position(), glm::vec2(0, 0), glm::vec2(LEVEL_WIDTH, LEVEL_HEIGHT));
}