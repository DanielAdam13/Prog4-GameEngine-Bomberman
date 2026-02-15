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
		
		void Update();
		void Render() const;

		template<typename T>
		T* AddComponent()
		{
			constexpr ComponentTypeID id{ T::StaticTypeID };

			// Skip if already existing
			if (m_Components[id] != nullptr)
				return static_cast<T*> (m_Components[id]);
			
			m_Components[id] = new T();
			return static_cast<T*> (m_Components[id]);
		}

		template<typename T>
		T* GetComponent() const
		{
			return static_cast<T*>(m_Components[T::StaticTypeID]);
		}

		const std::string GetName() const { return m_GameObjectName; }

	private:
		Transform* m_Transform;
		const std::string m_GameObjectName;

		std::array<Component*, MAX_GO_COMPONENTS> m_Components{};

	};
}