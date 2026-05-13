#pragma once
#include "Scene.h"
#include "Singleton.h"
#include "ObservableSubject.h"

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

namespace ge
{
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene(const std::string& sceneName);

		void FixedUpdate(const float fixedTimeStep);
		void Update(float deltaTime);
		void Render();

		Scene* GetCurrentActiveScene() const;
		void SwitchToSceneWithName(const std::string& sceneName);
		bool RemoveSceneWithName(const std::string& sceneName);

	private:
		// For GetInstance()
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		//std::vector<std::unique_ptr<Scene>> m_Scenes{};
		std::unordered_map<std::string, std::unique_ptr<Scene>> m_ScenesContainer{};

		Scene* m_CurrentActiveScene;
	};
}
