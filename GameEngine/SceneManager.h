#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Scene.h"
#include "Singleton.h"

namespace ge
{
	class Scene;

	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene();

		void FixedUpdate(const float fixedTimeStep);
		void Update(float deltaTime);
		void Render();

		Scene* GetCurrentScene() const;

	private:
		// For GetInstance()
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::vector<std::unique_ptr<Scene>> m_Scenes{};
	};
}
