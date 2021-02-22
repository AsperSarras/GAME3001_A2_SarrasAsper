#pragma once
#ifndef ___Sship___
#define ___Sship___

#include "DisplayObject.h"
#include "TextureManager.h"

class Sship : public DisplayObject
{
public:
	Sship();

	~Sship();

	// Inherited via GameObject
	void draw() override;
	void update() override;
	void clean() override;

	void setDestination(glm::vec2 destination);
	void setMaxSpeed(float speed);
	void setOrientation(glm::vec2 orientation);
	void setRotationAngle(float angle);
	float getRotation();

private:
	glm::vec2 m_destination;
	glm::vec2 m_targetDirection;
	glm::vec2 m_orientation; 
	float m_rotationAngle;
	float m_maxSpeed;

	void m_Move(); 

};
#endif /* defined (___Sship___)*/