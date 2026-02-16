#include "GameObject.h"
#include "Transform.h"
#include "Image.h"

using namespace ge;

GameObject::GameObject(const std::string& name)
	:m_pTransform{ nullptr },
	m_GameObjectName{ name }
{
	AddComponent<Transform>();
	m_pTransform = GetComponent<Transform>();
}

GameObject::~GameObject()
{
}

void GameObject::FixedUpdate(float fixedTimeStep)
{
	// Update ALL components except Transform
	for (const auto& comp : m_Components)
	{
		if (comp && comp.get() != m_pTransform)
		{
			comp->FixedUpdateComponent(fixedTimeStep);
		}
	}
}

void GameObject::Update(float deltaTime)
{
	// Update Transform Seperately
	//m_pTransform->SetPosition(m_pTransform->GetPosition() + glm::vec3{20.f, 0.f, 0.f} * deltaTime);

	// Update ALL components except Transform
	for (const auto& comp : m_Components)
	{
		if (comp && comp.get() != m_pTransform)
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