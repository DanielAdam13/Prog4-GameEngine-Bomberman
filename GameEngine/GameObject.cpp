#include "GameObject.h"
using namespace ge;

ge::GameObject::GameObject()
{
}

GameObject::~GameObject() = default;

void GameObject::Update() {}

void GameObject::Render() const
{
	//const auto& pos = m_transform.GetPosition();
	//Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
}