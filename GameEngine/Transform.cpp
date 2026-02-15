#include "Transform.h"

using namespace ge;

Transform::Transform()
	:m_Position{},
	m_Rotation{},
	m_Scale{ 1.f, 1.f, 1.f }
{
}

Transform::Transform(const glm::vec3 pos, const glm::vec3 rot, const glm::vec3 scale)
	:m_Position{ pos },
	m_Rotation{ rot },
	m_Scale{ scale }
{
}

void Transform::SetPosition(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void Transform::SetPosition(const glm::vec3& position)
{
	m_Position = position;
}

void ge::Transform::SetRotation(float x, float y, float z)
{
	m_Rotation.x = x;
	m_Rotation.y = y;
	m_Rotation.z = z;
}

void ge::Transform::SetRotation(const glm::vec3& rotation)
{
	m_Rotation = rotation;
}

void ge::Transform::SetScale(float x, float y, float z)
{
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;
}

void ge::Transform::SetScale(const glm::vec3& scale)
{
	m_Scale = scale;
}
