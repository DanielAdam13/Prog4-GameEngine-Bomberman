#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <string>

namespace ge
{
	// Type alias used for Unique IDs for each component TYPE
	using ComponentTypeID = uint32_t;

	class GameObject;

	class Component
	{
	public:
		Component(GameObject* pOwnerPtr)
			:m_pOwner{ pOwnerPtr },
			m_DeletionMark{ false }
		{};

		virtual ~Component() = default;
		Component(const Component& other) = delete;
		Component(Component&& other) = delete;
		Component& operator=(const Component& other) = delete;
		Component& operator=(Component&& other) = delete;

		// Every Component HAS to override:
		virtual void FixedUpdateComponent(float) = 0;
		virtual void UpdateComponent(float) = 0;
		virtual void RenderComponent() const = 0;

		GameObject* GetOwner() const noexcept { return m_pOwner; }

		void MarkForDeletion() { m_DeletionMark = true; }
		bool MarkedForDeletion() const noexcept { return m_DeletionMark; }

	protected:
		// Empty virtual methods, Component CAN override, do not NEED to
		virtual void OnCollisionEnter(ge::GameObject*, const std::string&) {}
		virtual void OnCollisionExit(ge::GameObject*, const std::string&) {}
		
	private:
		// Private so derived classes cannot change, Initialized ONLY on construction
		GameObject* m_pOwner; // Doesn't own

		bool m_DeletionMark;
	};
}