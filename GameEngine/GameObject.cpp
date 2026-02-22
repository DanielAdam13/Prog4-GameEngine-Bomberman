#include "GameObject.h"
#include "Transform.h"
#include "Image.h"

using namespace ge;

GameObject::GameObject(const std::string& name)
	:m_pTransform{ nullptr },
	m_GameObjectName{ name },
	m_DeletionMark{ false },
	m_Parent{ nullptr },
	m_Children{}
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

void GameObject::MarkForDeletion()
{
	if (m_DeletionMark)
		return;

	// 1. Apply mark
	m_DeletionMark = true;

	// 2. Remove parent reference
	if (m_Parent)
	{
		m_Parent->RemoveChild(this);
		m_Parent = nullptr;
	}

	// 3. Mark ALL children
	for (auto* child : m_Children)
	{
		// RECURSION, will call itself for child's children
		if (child)
			child->MarkForDeletion();
	}

	// 4. Clear vector
	m_Children.clear();
}

void GameObject::SetParent(GameObject* newParent, bool keepWorldPos)
{
	// 1. Is new parent valid?
	if (newParent == this || m_Parent == newParent || 
		this->ContainsChild(newParent)) // new parent is not a child of the current m_Parent
		return;

	// 2. Handle Local and World pos:
	if (newParent == nullptr)
	{
		// Local = World
	}
	else
	{
		if (keepWorldPos)
		{
			// Update Local = World - parent.World
		}
	}

	// 3. Remove itself as a child from the OLD parent
	if (m_Parent)
		m_Parent->RemoveChild(this);

	// 4. Set NEW parent to this parent
	m_Parent = newParent;

	// 5. Add itself as achild of the NEW parent
	if (m_Parent)
		m_Parent->AddChild(this);
}

GameObject* GameObject::GetChildByID(const unsigned int index) const
{
	if (index >= m_Children.size())
		return nullptr;

	const auto& obj = m_Children[index];

	if (!obj || obj->MarkedForDeletion()) // Checks for lifetime availability
		return nullptr;

	return obj;
}

GameObject* GameObject::GetChildByName(const std::string& childName) const
{
	for (auto* child : m_Children) // O(n)
	{
		if (child && child->GetName() == childName)
			return child;
	}

	return nullptr;
}

bool GameObject::ContainsChild(GameObject* parent) const
{
	if (!parent)
		return false;

	for (const auto& child : m_Children)
	{
		if (child == parent) // Direct child
			return true;

		// Recursion
		if (child->ContainsChild(parent)) // Child of child
			return true;
	}

	return false;
}

void GameObject::AddChild(GameObject* child)
{
	if (!child || child == this)
		return;
	if (child->IsAncestor(m_Parent)) // Make sure CHILD is not a parent of the parent from which this is called 
		return;

	// Make sure not already a child of current - not deeper child
	for (const auto& ch : m_Children)
	{
		if (child == ch) // Direct child
			return;
	}

	// Set child's parent to THIS game object
	child->m_Parent = this;

	m_Children.push_back(child);
}

void GameObject::RemoveChild(GameObject* child)
{
	if (!child || child == this || child->IsAncestor(this))
		return;

	// Find index of child
	auto it = std::find(m_Children.begin(), m_Children.end(), child); // O(1) if unordered_set

	if (it != m_Children.end())
	{
		// Remove pointer from the vector
		m_Children.erase(it);

		// Remove this object as parent from the child
		if (child->m_Parent == this)
			child->m_Parent = nullptr;
	}
}

bool GameObject::IsAncestor(const GameObject* obj) const
{
	const GameObject* currentParent{ m_Parent };

	// Loop to a parent of a parent that potentially == obj
	while (currentParent)
	{
		if (currentParent == obj)
			return true;

		currentParent = currentParent->m_Parent; // go one step above
	}

	return false;
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
