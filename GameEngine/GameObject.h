#pragma once
#include <string>
#include <memory>
#include "Transform.h"
#include <array>
#include "Component.h"

#include <concepts>
#include <stdexcept>
//#include <unordered_set>

namespace ge
{
	static constexpr size_t MAX_GO_COMPONENTS{ 40 };

	class GameObject final
	{
	public:
		explicit GameObject(const std::string& name);
		~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;
		
		void FixedUpdate(float fixedTimeStep);
		void Update(float deltaTime);
		void Render() const;

		// Args for passing constructors on component addition
		template<std::derived_from<Component> T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			constexpr ComponentTypeID id{ T::StaticTypeID };

			// Early out component if already existing
			if (m_Components[id] != nullptr && !m_Components[id]->MarkedForDeletion()) 
				return static_cast<T*> (m_Components[id].get());
			
			// --- Custom Component constructors are executed ONLY if component doesn't exist already ---
			auto newComponent{ std::make_unique<T>(std::forward<Args>(args)...) }; // forwarding reference

			T* storedRawPtr{ newComponent.get() };

			m_Components[id] = std::move(newComponent); 
			return storedRawPtr;
		}

		template<std::derived_from<Component> T>
		T* GetComponent() const
		{
			auto* rawCompPtr{ m_Components[T::StaticTypeID].get() };

			if (!rawCompPtr || rawCompPtr->MarkedForDeletion()) // Checks for lifetime availability
				return nullptr;

			return static_cast<T*>(rawCompPtr);
		}

		template<std::derived_from<Component> T>
		void RemoveComponent()
		{
			constexpr ComponentTypeID id{ T::StaticTypeID };

			if (m_Components[id] == nullptr)
				return;

			m_Components[id]->MarkForDeletion();
		}

		template<std::derived_from<Component> T>
		bool HasComponent() const noexcept
		{
			return m_Components[T::StaticTypeID] != nullptr &&
				!m_Components[T::StaticTypeID]->MarkedForDeletion();
		}

		const std::string& GetName() const { return m_GameObjectName; }

		void MarkForDeletion();
		bool MarkedForDeletion() const noexcept { return m_DeletionMark; }

		void SetParent(GameObject* newParent, bool keepWorldPos = true);
		GameObject* GetParent() const;

		GameObject* GetChildByID(const unsigned int index) const; // O(1)
		GameObject* GetChildByName(const std::string& childName) const; // Slow - O(n)
		int GetChildrenCount() const;

	private:
		Transform* m_pTransform; // Reference to the transform component

		const std::string m_GameObjectName;

		std::array<std::unique_ptr<Component>, MAX_GO_COMPONENTS> m_Components{};

		void CleanupDestroyedComponents();

		bool m_DeletionMark;

		// Parent/Child relationship variables
		GameObject* m_Parent;
		std::vector<GameObject*> m_Children; // vector for faster Updates and Render + index accessing

		bool ContainsChild(GameObject* parent) const;
		void AddChild(GameObject* child);
		void RemoveChild(GameObject* child);
		bool IsAncestor(const GameObject* obj) const;

	};
}