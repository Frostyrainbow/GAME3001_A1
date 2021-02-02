#include "Jet.h"
#include "Util.h"
#include "Game.h"

Jet::Jet():m_maxSpeed(10.0f)
{
	TextureManager::Instance()->load("../Assets/textures/jet.png", "jet");

	auto size = TextureManager::Instance()->getTextureSize("jet");
	setWidth(size.x);
	setHeight(size.y);

	getTransform()->position = glm::vec2(400.0f, 300.0f);
	getRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	getRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	getRigidBody()->isColliding = false;
	setType(JET);
	setOrientation(glm::vec2(0.0f, -1.0f));
	setRotation(0.0f);
	setAccelerationRate(10.0f);
	setTurnRate(10.0f);
}

Jet::~Jet()
= default;

void Jet::draw()
{
	TextureManager::Instance()->draw("jet", getTransform()->position.x, getTransform()->position.y, getRotation(), 255, true);
	//Util::DrawLine(getTransform()->position, (getTransform()->position + m_orientation * 60.0f));
	if (m_state == 4)
	{
		Util::DrawLine(m_leftWhisker.Start(), m_leftWhisker.End());
		Util::DrawLine(m_rightWhisker.Start(), m_rightWhisker.End());
	}
	
}

void Jet::update()
{
	
	/*setWhisker(getTransform()->position, getTransform()->position + Util::getOrientation(m_rotationAngle + 45) * 100.0f);*/
	if (m_state == 1)
	{
		m_Seek();
	}
	else if (m_state == 2)
	{
		m_Flee();
	}
	else if (m_state == 3)
	{
		m_Arrival();
	}
	else if (m_state == 4)
	{
		m_leftWhisker.SetLine(getTransform()->position, getTransform()->position + Util::getOrientation(m_rotationAngle - 30) * 100.0f);
		m_rightWhisker.SetLine(getTransform()->position, getTransform()->position + Util::getOrientation(m_rotationAngle + 30) * 100.0f);
		m_CollisionAvoid();
	}
	
}

void Jet::clean()
{

}

float Jet::getRotation()
{
	return m_rotationAngle;
}

glm::vec2 Jet::getOrientation()
{
	return m_orientation;
}

float Jet::getTurnRate()
{
	return m_turnRate;
}

float Jet::getAccelerationRate()
{
	return m_accelerationRate;
}

int Jet::getState()
{
	return m_state;
}

bool Jet::getArivied()
{
	return m_arivied;
}

void Jet::setWhisker(glm::vec2 start, glm::vec2 end)
{
	m_leftWhisker.SetLine(start, end);
}

void Jet::setDestination(const glm::vec2 destination)
{
	m_destination = destination;
}

void Jet::setMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void Jet::setOrientation(const glm::vec2 orientation)
{
	m_orientation = orientation;
}

void Jet::setRotation(float angle)
{
	m_rotationAngle = angle;
	auto offset = -90.0f;
	auto angle_in_radians = (angle + offset) * Util::Deg2Rad;

	auto x = cos(angle_in_radians);
	auto y = sin(angle_in_radians);

	//convert the angle to a normalized vector and store it in orientation
	setOrientation(glm::vec2(x, y));
}

void Jet::setTurnRate(float rate)
{
	m_turnRate = rate;
}

void Jet::setAccelerationRate(float rate)
{
	m_accelerationRate = rate;
}

void Jet::setState(int state)
{
	m_state = state;
}

void Jet::setArivied(bool arivied)
{
	m_arivied = arivied;
}

void Jet::m_Seek()
{
	auto deltaTime = TheGame::Instance()->getDeltaTime();
	
	// direction with magnitude
	m_targetDirection = m_destination - getTransform()->position;
	// normalized direction
	m_targetDirection = Util::normalize(m_targetDirection);

	auto target_rotation = Util::signedAngle(getOrientation(), m_targetDirection);

	auto turn_sensitivity = 5.0f;

	if (abs(target_rotation) > turn_sensitivity)
	{
		if (target_rotation > 0.0f)
		{
			setRotation(getRotation() + getTurnRate());
		}
		else if (target_rotation < 0.0f)
		{
			setRotation(getRotation() - getTurnRate());
		}
	}

	
	getRigidBody()->acceleration = getOrientation() * getAccelerationRate();

	getRigidBody()->velocity += getOrientation() * (deltaTime)+0.5f * getRigidBody()->acceleration * (deltaTime);

	getRigidBody()->velocity = Util::clamp(getRigidBody()->velocity, m_maxSpeed);

	getTransform()->position += getRigidBody()->velocity;

}

void Jet::m_Flee()
{
	auto deltaTime = TheGame::Instance()->getDeltaTime();

	// direction with magnitude
	m_targetDirection = -m_destination - getTransform()->position;
	// normalized direction
	m_targetDirection = Util::normalize(m_targetDirection);

	auto target_rotation = Util::signedAngle(getOrientation(), m_targetDirection);

	auto turn_sensitivity = 5.0f;

	if (abs(target_rotation) > turn_sensitivity)
	{
		if (target_rotation > 0.0f)
		{
			setRotation(getRotation() + getTurnRate());
		}
		else if (target_rotation < 0.0f)
		{
			setRotation(getRotation() - getTurnRate());
		}
	}

	getRigidBody()->acceleration = getOrientation() * getAccelerationRate();

	getRigidBody()->velocity += getOrientation() * (deltaTime)+0.5f * getRigidBody()->acceleration * (deltaTime);

	getRigidBody()->velocity = Util::clamp(getRigidBody()->velocity, m_maxSpeed);

	getTransform()->position += getRigidBody()->velocity;

}

void Jet::m_Arrival()
{
	m_Seek();
}

void Jet::m_CollisionAvoid()
{
	m_Seek();
}
