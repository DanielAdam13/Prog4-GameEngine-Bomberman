#include "GameObject.h"
#include "Transform.h"
#include "Image.h"

using namespace ge;

GameObject::GameObject(const std::string& name)
	:m_pTransform{ nullptr },
	m_GameObjectName{ name }
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
		if (comp)
		{
			comp->FixedUpdateComponent(fixedTimeStep);
		}
	}
}

void GameObject::Update(float deltaTime)
{
	for (const auto& comp : m_Components)
	{
		if (comp)
		{
			comp->UpdateComponent(deltaTime);
		}
	}
}

void GameObject::Render() const
{
	const auto& transformPos = m_pTransform->GetPosition();

	for (const auto& comp : m_Components)
	{
		if (comp)
		{
			comp->RenderComponent(transformPos);
		}
	}
}