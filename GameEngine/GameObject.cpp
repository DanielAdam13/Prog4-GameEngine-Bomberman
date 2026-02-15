#include "GameObject.h"
#include "Transform.h"
#include "Image.h"

using namespace ge;

#include "Renderer.h"

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
	m_Transform->SetPosition({ m_Transform->GetPosition().x + 1.f, 0.f, 0.f});
}

void GameObject::Render() const
{
	const auto& pos = m_Transform->GetPosition();
	Renderer::GetInstance().RenderTexture(*GetComponent<Image>()->GetTexture(), pos.x, pos.y);
}