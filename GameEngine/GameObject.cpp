#include "GameObject.h"
#include "Transform.h"
#include "Image.h"

using namespace ge;

GameObject::GameObject(const std::string& name)
	:m_Transform{},
	m_GameObjectName{ name }
{
	AddComponent<Transform>();
	m_Transform = GetComponent<Transform>();
}

GameObject::~GameObject()
{
}

void GameObject::Update() 
{
	// Update Transform Seperately
	//m_Transform->SetPosition({ m_Transform->GetPosition().x + 1.f, 0.f, 0.f});

	// Update ALL components except Transform
	for (auto* comp : m_Components)
	{
		if (comp && comp != m_Transform)
		{
			comp->UpdateComponent();
		}
	}
}

void GameObject::Render() const
{
	const auto& transformPos = m_Transform->GetPosition();

	for (auto* comp : m_Components)
	{
		if (comp)
		{
			comp->RenderComponent(transformPos);
		}
	}
}