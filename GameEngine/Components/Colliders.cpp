#include "Colliders.h"
#include "CollisionSystem.h"
#include "EngineEvents.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "Renderer.h" // for drawing the debugging

#include <SDL3/SDL.h> // for debugging
#include <cassert>

ge::Collider::Collider(GameObject* pOwnerPtr, Shape shape)
	:Component::Component(pOwnerPtr),
	m_Shape{ shape },
	m_CachedOwnerTransform{ pOwnerPtr->GetComponent<ge::Transform>() }
{
	const auto& allLayerTags{ CollisionSystem::GetInstance().GetLayerTags() };
	auto it{ allLayerTags.find("Default") };
	if (it != allLayerTags.end())
	{
		m_LayerTag = *it;
	}
}

ge::Transform* ge::Collider::GetOwnerTransform() const noexcept
{
	return m_CachedOwnerTransform;
}

ge::Subject& ge::Collider::GetOnCollisionEnterEvent() noexcept
{
	return m_OnCollisionEnterEvent;
}

ge::Subject& ge::Collider::GetOnCollisionExitEvent() noexcept
{
	return m_OnCollisionExitEvent;
}

ge::Collider::Shape ge::Collider::GetShape() const noexcept
{
	return m_Shape;
}

void ge::Collider::BeginOverlap()
{
	//m_CurrentOverlapping.swap(m_PreviousOverlapping);
	std::swap(m_CurrentOverlapping, m_PreviousOverlapping);

	m_CurrentOverlapping.clear();
}

void ge::Collider::RecordOverlap(Collider* other)
{
	m_CurrentOverlapping.insert(other); // no duplicates
}

void ge::Collider::EndOverlap()
{
	for (auto* current : m_CurrentOverlapping)
	{
		if (!m_PreviousOverlapping.contains(current))
		{
			m_OnCollisionEnterEvent.NotifyObservers(EngineEventId::COLLISION_ENTER, current->GetOwner());
		}
	}
	for (auto* prev : m_PreviousOverlapping)
	{
		if (!m_CurrentOverlapping.contains(prev))
		{
			m_OnCollisionExitEvent.NotifyObservers(EngineEventId::COLLISION_EXIT, prev->GetOwner());
		}
	}
}

bool ge::Collider::IsOverlapping(Collider* other) const
{
	return m_CurrentOverlapping.contains(other);
}

void ge::Collider::AssignTag(const CollisionLayerTag& tag)
{
	const auto& allLayerTags{ CollisionSystem::GetInstance().GetLayerTags() };
	auto it{ allLayerTags.find(tag) };

	if (it == allLayerTags.end())
	{
		assert(false && "AssignTag: tag does not exist in CollisionSystem");
		return;
	}
	m_LayerTag = *it;
}

const ge::CollisionLayerTag& ge::Collider::GetLayerTag() const noexcept
{
	return m_LayerTag;
}

ge::BoxCollider::BoxCollider(GameObject* pOwnerPtr, const glm::vec2& size, bool ignoreOwnerSize, const glm::vec2& localOffset)
	:Collider::Collider(pOwnerPtr, Shape::Box),
	m_Size{ size },
	m_LocalOffset{ localOffset },
	m_IgnoreOwnerSize{ ignoreOwnerSize }
{
	CollisionSystem::GetInstance().Register(this);
}

ge::BoxCollider::~BoxCollider()
{
	CollisionSystem::GetInstance().Unregister(this);
}

void ge::BoxCollider::RenderComponent() const
{
#if _DEBUG
	const auto bounds{ GetBounds() };
	Renderer::GetInstance().RenderRectOutline(
		bounds.position.x, bounds.position.y,
		bounds.size.x, bounds.size.y,
		SDL_Color{ 255, 0, 0, 255 });
#endif
}

ge::structs::Rect ge::BoxCollider::GetBounds() const noexcept
{
	// Recompute only when GetBounds CALLED
	auto* tf{ GetOwner()->GetComponent<ge::Transform>() };
	const glm::vec3 worldPos{ tf->GetWorldPosition() };

	glm::vec3 worldScale{ tf->GetWorldScale() };
	if (m_IgnoreOwnerSize)
		worldScale = { 1.f, 1.f, 1.f };

	const glm::vec2 worldSize{ m_Size.x * worldScale.x, m_Size.y * worldScale.y };

	return structs::Rect{ 
		glm::vec2{worldPos.x, worldPos.y} + m_LocalOffset,
		worldSize };
}

ge::structs::Rect ge::BoxCollider::GetBoundsAt(const glm::vec3& worldPos) const noexcept
{
	auto worldScale{ GetOwnerTransform()->GetWorldScale() };
	if (m_IgnoreOwnerSize)
		worldScale = { 1.f, 1.f, 1.f };

	const glm::vec2 worldSize{ m_Size.x * worldScale.x, m_Size.y * worldScale.y };
	
	ge::structs::Rect hyphoteticalRect{ glm::vec2{worldPos.x, worldPos.y} + m_LocalOffset, worldSize };
	return hyphoteticalRect;
}

ge::CircleCollider::CircleCollider(GameObject* pOwnerPtr, const structs::Circle& circle)
	:Collider::Collider(pOwnerPtr, Shape::Circle),
	m_BoundCircle{ circle }
{
	CollisionSystem::GetInstance().Register(this);
}

ge::CircleCollider::~CircleCollider()
{
	CollisionSystem::GetInstance().Unregister(this);
}

ge::structs::Circle ge::CircleCollider::GetBounds() const noexcept
{
	return m_BoundCircle;
}

// ============================================
// Overlap Helpers:
// ============================================
bool ge::collisionHelpers::Overlaps(const structs::Rect& a, const structs::Rect& b)
{
	return a.position.x < b.position.x + b.size.x &&
		a.position.x + a.size.x > b.position.x &&
		a.position.y < b.position.y + b.size.y &&
		a.position.y + a.size.y > b.position.y;
}

bool ge::collisionHelpers::Overlaps(const structs::Circle& a, const structs::Circle& b)
{
	const glm::vec2 delta{ a.centerPos - b.centerPos };
	const float distSqr{ glm::dot(delta, delta) };
	const float radiusSum{ a.radius + b.radius };
	return distSqr < radiusSum * radiusSum;
}

bool ge::collisionHelpers::Overlaps(const structs::Rect& a, const structs::Circle& b)
{
	const glm::vec2 rectMin{ a.position };
	const glm::vec2 rectMax{ a.position + a.size };
	const glm::vec2 closest{ glm::clamp(b.centerPos, rectMin, rectMax) };

	const glm::vec2 delta{ b.centerPos - closest };
	const float distSqr{ glm::dot(delta, delta) };
	return distSqr <= b.radius * b.radius;
}
