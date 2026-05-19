#pragma once

#include "Component.h"
#include "Structs.h"
#include "ObservableSubject.h"

#include <unordered_set>
#include <string>

namespace ge
{
	using CollisionLayerTag = std::string;

	class Transform;

	// No outside source can initialize base Collider, only dervived classes !!
	class Collider : public Component
	{
	public:
		Collider() = delete; // !!
		virtual ~Collider() = default;
		Collider(const Collider& other) = delete;
		Collider(Collider&& other) = delete;
		Collider& operator=(const Collider& other) = delete;
		Collider& operator=(Collider&& other) = delete;

		virtual void FixedUpdateComponent(float) {}
		virtual void UpdateComponent(float) {}
		virtual void RenderComponent() const {}

		Subject& GetOnCollisionEnterEvent() noexcept;
		Subject& GetOnCollisionExitEvent() noexcept;

		// Shape Tag for colliders
		enum class Shape {
			Box,
			Circle
		};
		Shape GetShape() const noexcept;

		void BeginOverlap();
		void RecordOverlap(Collider* other);
		void EndOverlap();
		bool IsOverlapping(Collider* other) const;

		void AssignTag(const CollisionLayerTag& tag);
		const CollisionLayerTag& GetLayerTag() const noexcept;

	protected:
		Collider(GameObject* pOwnerPtr, Shape shape);
		Transform* GetOwnerTransform() const noexcept;

	private:
		Shape m_Shape;
		CollisionLayerTag m_LayerTag;

		Subject m_OnCollisionEnterEvent;
		Subject m_OnCollisionExitEvent;

		std::unordered_set<Collider*> m_CurrentOverlapping;
		std::unordered_set<Collider*> m_PreviousOverlapping;

		Transform* m_CachedOwnerTransform;
	};

	class BoxCollider final : public Collider
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every BoxCollider Instance shares the same component type ID
		static constexpr ComponentTypeID StaticTypeID{ 14 };

		BoxCollider(GameObject* pOwnerPtr, const glm::vec2& size, bool ignoreOwnerSize = false, const glm::vec2& localOffset = {});
		~BoxCollider() override;
		BoxCollider(const BoxCollider& other) = delete;
		BoxCollider(BoxCollider&& other) = delete;
		BoxCollider& operator=(const BoxCollider& other) = delete;
		BoxCollider& operator=(BoxCollider&& other) = delete;

		virtual void FixedUpdateComponent(float) override {}
		virtual void UpdateComponent(float) override {}
		virtual void RenderComponent() const override;

		structs::Rect GetBounds() const noexcept;
		// Used for hyphotetical bounds with the parameter acting as Rect::position
		structs::Rect GetBoundsAt(const glm::vec3& worldPos) const noexcept;

	private:
		glm::vec2 m_Size;
		glm::vec2 m_LocalOffset;

		bool m_IgnoreOwnerSize;
	};

	class CircleCollider final : public Collider
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every CircleCollider Instance shares the same component type ID
		static constexpr ComponentTypeID StaticTypeID{ 15 };

		CircleCollider(GameObject* pOwnerPtr, const structs::Circle& circle);
		~CircleCollider() override;
		CircleCollider(const CircleCollider& other) = delete;
		CircleCollider(CircleCollider&& other) = delete;
		CircleCollider& operator=(const CircleCollider& other) = delete;
		CircleCollider& operator=(CircleCollider&& other) = delete;

		virtual void FixedUpdateComponent(float) override {}
		virtual void UpdateComponent(float) override {}
		virtual void RenderComponent() const override {}

		structs::Circle GetBounds() const noexcept;

	private:
		structs::Circle m_BoundCircle;

	};

	namespace collisionHelpers
	{
		bool Overlaps(const structs::Rect& a, const structs::Rect& b);
		bool Overlaps(const structs::Circle& a, const structs::Circle& b);
		bool Overlaps(const structs::Rect& a, const structs::Circle& b);
		inline bool Overlaps(const structs::Circle& a, const structs::Rect& b) 
		{
			return Overlaps(b, a);
		}
	}
}