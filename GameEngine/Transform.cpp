#include "Transform.h"
#include "GameObject.h"
#include <glm/ext/matrix_transform.inl>

using namespace ge;

Transform::Transform(GameObject* pOwnerPtr)
	:Transform::Transform(pOwnerPtr, {}, {}, {1.f, 1.f, 1.f})
{
}

Transform::Transform(GameObject* pOwnerPtr, const glm::vec3 pos, const glm::vec3 rot, const glm::vec3 scale)
	:Component::Component(pOwnerPtr),
	m_LocalPosition{ pos },
	m_LocalRotation{ rot },
	m_LocalScale{ scale },
	m_DirtyFlag{ true }
{
}

glm::vec3 Transform::GetWorldPosition()
{
	RecomputeWorldMatrix();
	return glm::vec3(m_WorldMatrix[3]);
}

glm::vec3 Transform::GetWorldRotation()
{
	RecomputeWorldMatrix();
	return {};
}

glm::vec3 Transform::GetWorldScale()
{
	RecomputeWorldMatrix();
	glm::vec3 scale{};
	scale.x = glm::length(glm::vec3(m_WorldMatrix[0]));
	scale.y = glm::length(glm::vec3(m_WorldMatrix[1]));
	scale.z = glm::length(glm::vec3(m_WorldMatrix[2]));
	return scale;
}

const glm::mat4x4& Transform::GetWorldMatrix()
{
	RecomputeWorldMatrix();
	return m_WorldMatrix;
}

void Transform::SetLocalPosition(float x, float y, float z)
{
	m_LocalPosition.x = x;
	m_LocalPosition.y = y;
	m_LocalPosition.z = z;
	MarkDirty();
}

void Transform::SetLocalPosition(const glm::vec3& position)
{
	m_LocalPosition = position;
	MarkDirty();
}

void Transform::SetLocalRotation(float x, float y, float z)
{
	m_LocalRotation.x = x;
	m_LocalRotation.y = y;
	m_LocalRotation.z = z;
	MarkDirty();
}

void Transform::SetLocalRotation(const glm::vec3& rotation)
{
	m_LocalRotation = rotation;
	MarkDirty();
}

void Transform::SetLocalScale(float x, float y, float z)
{
	m_LocalScale.x = x;
	m_LocalScale.y = y;
	m_LocalScale.z = z;
	MarkDirty();
}

void Transform::SetLocalScale(const glm::vec3& scale)
{
	m_LocalScale = scale;
	MarkDirty();
}

Transform* Transform::GetParentTransform() const
{
	auto parent{ GetOwner()->GetParent() };

	if (!parent || parent->MarkedForDeletion())
		return nullptr;

	return parent->GetComponent<Transform>();
}

void Transform::MarkDirty()
{
	if (m_DirtyFlag)
		return;

	m_DirtyFlag = true;

	// Mark all children of owner as well
	auto owner{ GetOwner() };
	for (int i{}; i < owner->GetChildrenCount(); ++i)
	{
		auto child{ owner->GetChildByID(i) };

		// RECURSION between children
		if (child && !child->MarkedForDeletion())
		{
			child->GetComponent<Transform>()->MarkDirty();
		}
	}
}

void Transform::RecomputeWorldMatrix() 
{
	// IF DIRTY
	if (m_DirtyFlag)
	{
		auto parent{ GetParentTransform() };

		// 1. Recompute parent's world matrix
		if (parent)
		{
			parent->RecomputeWorldMatrix();
		}

		// 2. Rebuild Local TRS matrix
		const glm::mat4 T{glm::translate(glm::mat4(1.f), m_LocalPosition)};

		const glm::mat4 rotX{ glm::rotate(glm::mat4(1.0f), m_LocalRotation.x, glm::vec3(1, 0, 0)) };
		const glm::mat4 rotY{ glm::rotate(glm::mat4(1.0f), m_LocalRotation.y, glm::vec3(0, 1, 0)) };
		const glm::mat4 rotZ{ glm::rotate(glm::mat4(1.0f), m_LocalRotation.z, glm::vec3(0, 0, 1)) };
		const glm::mat4 R{ rotZ * rotY * rotX }; // ZYX rotation order -> yaw/pitch/row

		const glm::mat4 S{ glm::scale(glm::mat4(1.f), m_LocalScale) };

		m_LocalMatrix = T * R * S;

		// 3. Finally, update world matrix
		if (parent)
			m_WorldMatrix = parent->GetWorldMatrix() * m_LocalMatrix;
		else
			m_WorldMatrix = m_LocalMatrix;
			

		m_DirtyFlag = false;
	}
}

void Transform::SetLocalFromMatrix(const glm::mat4& m)
{
	// position
	m_LocalPosition = glm::vec3(m[3]);

	// rotation/scale ignored for now

	MarkDirty();
}