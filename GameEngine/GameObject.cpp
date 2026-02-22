#include "GameObject.h"
#include "Transform.h"
#include "Image.h"

using namespace ge;

GameObject::GameObject(const std::string& name)
	:m_pTransform{ nullptr },
	m_GameObjectName{ name },
	m_DeletionMark{ false }
{
	AddComponent<Transform>(this);
	m_pTransform = GetComponent<Transform>();
}

GameObject::~GameObject()
{
}

void GameObject::FixedUpdate(float fixedTimeStep)
{
	for (const auto& comp : m_Components)
	{
		if (comp && !comp->MarkedForDeletion())
		{
			comp->FixedUpdateComponent(fixedTimeStep);
		}
	}

	// Destroy marked components
	CleanupDestroyedComponents();
}

void GameObject::Update(float deltaTime)
{
	for (const auto& comp : m_Components)
	{
		if (comp && !comp->MarkedForDeletion())
		{
			comp->UpdateComponent(deltaTime);
		}
	}

	// Destroy marked components
	CleanupDestroyedComponents();
}

void GameObject::Render() const
{
	const auto& transformPos = m_pTransform->GetPosition();

	for (const auto& comp : m_Components)
	{
		if (comp && !comp->MarkedForDeletion())
		{
			comp->RenderComponent(transformPos);
		}
	}
}

void GameObject::CleanupDestroyedComponents()
{
	for (auto& comp : m_Components)
	{
		if (!comp || comp->MarkedForDeletion())
		{
			comp.reset(); // Safely delete unique ptr
		}
	}
}
