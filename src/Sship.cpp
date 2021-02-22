#include "Sship.h"
#include "Util.h"

Sship::Sship()
{
	TextureManager::Instance()->load("../Assets/textures/Sship.png", "Sship");

	auto size = TextureManager::Instance()->getTextureSize("Sship");
	setWidth(size.x);
	setHeight(size.y);

	getTransform()->position = glm::vec2(400.0f, 300.0f);
	getRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	getRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	getRigidBody()->isColliding = false;
	setType(SShip);
	setMaxSpeed(10.0f);
	setOrientation(glm::vec2(0.0f, -1.0f));
	setRotationAngle(0.0f);
}

Sship::~Sship()
= default;

void Sship::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the ship
	TextureManager::Instance()->draw("Sship", 
		getTransform()->position.x, getTransform()->position.y,m_rotationAngle, 255, true);

	Util::DrawLine(getTransform()->position, (getTransform()->position + m_orientation * 60.0f));
}

void Sship::update()
{
	m_Move();
}

void Sship::clean()
{
}

void Sship::setDestination(const glm::vec2 destination)
{
	m_destination = destination;
}

void Sship::setMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void Sship::setOrientation(glm::vec2 orientation)
{
	m_orientation = orientation;
}

void Sship::setRotationAngle(float angle)
{
	m_rotationAngle = angle;
}

float Sship::getRotation()
{
	return m_rotationAngle;
}

void Sship::m_Move()
{
	//Magnitude
	m_targetDirection = m_destination - getTransform()->position;
	//Normalized direction
	m_targetDirection = Util::normalize(m_targetDirection); 

	getRigidBody()->velocity = m_targetDirection * m_maxSpeed;

	getTransform()->position += getRigidBody()->velocity;
}
