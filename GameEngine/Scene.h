#pragma once
#include <memory>
#include <string>
#include <vector>
#include "GameObject.h"

namespace ge
{
	class Scene final
	{
	public:
		void Add(std::unique_ptr<GameObject> object);
		void Remove(const GameObject& object);
		void RemoveAll();

		void FixedUpdate(float fixedTimeStep);
		void Update(float deltaTime);
		void Render() const;

		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		GameObject* FindObjectByName(const std::string& goName) const;
		GameObject* FindObjectByID(const size_t index) const;

		void RemoveObjectByName(const std::string& goName);
		void RemoveObjectByID(const size_t index);

	private:
		friend class SceneManager;
		explicit Scene() = default;

		std::vector <std::unique_ptr<GameObject>> m_Objects{};

		void CleanupDestroyedGameObjects();
	};

}
