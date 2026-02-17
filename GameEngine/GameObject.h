#pragma once
#include <string>
#include <memory>
#include "Transform.h"
#include <array>
#include "Component.h"

namespace ge
{
	static constexpr size_t MAX_GO_COMPONENTS{ 20 };

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
		template<typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			constexpr ComponentTypeID id{ T::StaticTypeID };

			if (m_Components[id] != nullptr) // Early out component if already existing
				return static_cast<T*> (m_Components[id].get());
			
			// --- Custom Component constructors are executed ONLY if component doesn't exist already ---
			auto newComponent{ std::make_unique<T>(std::forward<Args>(args)...) }; // forwarding reference

			T* storedRawPtr{ newComponent.get() };

			// Assign owner pointer reference to every component
			storedRawPtr->SetOwner(this);

			m_Components[id] = std::move(newComponent); 
			return storedRawPtr;
		}

		template<typename T>
		T* GetComponent() const
		{
			return static_cast<T*>(m_Components[T::StaticTypeID].get());
		}

		const std::string GetName() const { return m_GameObjectName; }

	private:
		Transform* m_pTransform;

		// Main way to identify a game object
		const std::string m_GameObjectName;

		std::array<std::unique_ptr<Component>, MAX_GO_COMPONENTS> m_Components{};

	};
}