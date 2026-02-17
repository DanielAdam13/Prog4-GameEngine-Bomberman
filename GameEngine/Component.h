#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace ge
{
	// Type alias used for Unique IDs for each component TYPE
	using ComponentTypeID = uint32_t;

	class GameObject;

	class Component
	{
	public:
		Component() = default;
		virtual ~Component() = default;

		Component(const Component& other) = delete;
		Component(Component&& other) = delete;
		Component& operator=(const Component& other) = delete;
		Component& operator=(Component&& other) = delete;

		// Every Component HAS to override:
		virtual void FixedUpdateComponent(float) = 0;
		virtual void UpdateComponent(float) = 0;
		virtual void RenderComponent(const glm::vec3&) const = 0;

		GameObject* GetOwner() const noexcept { return m_pOwner; }

		void SetOwner(GameObject* p) { m_pOwner = p; }

	protected:
		// Reference to owner
		GameObject* m_pOwner{ nullptr }; // Doesn't own

		// Only GameObject assigns ownership
		friend class GameObject;
	};
}