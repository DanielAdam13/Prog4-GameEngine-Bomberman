#pragma once
#include "GameObject.h"
#include "ImGuiScene.h"

#include <memory>
#include <string>
#include <vector>

namespace ge
{
	class Scene final
	{
	public:
		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		void Add(std::unique_ptr<GameObject> object);
		void Remove(const GameObject& object);
		void RemoveAll();

		void FixedUpdate(float fixedTimeStep);
		void Update(float deltaTime);
		void Render() const;

		GameObject* FindObjectByName(const std::string& goName) const;
		GameObject* FindObjectByID(const size_t index) const;

		void RemoveObjectByName(const std::string& goName);
		void RemoveObjectByID(const unsigned int index);

		// ---- ImGui ----
		void AddImGuiScene(std::unique_ptr<ImGuiInstance> imguiScene);

		std::string GetSceneName() const noexcept;

	private:
		friend class SceneManager;
		Scene(const std::string& sceneName);

		std::string m_SceneName;

		std::vector <std::unique_ptr<GameObject>> m_Objects{};

		void CleanupDestroyedGameObjects();

		std::vector<std::unique_ptr<ImGuiInstance>> m_ImGuiInstances{};
	};

}
