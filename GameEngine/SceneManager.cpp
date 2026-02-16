#include "SceneManager.h"
#include "Scene.h"

using namespace ge;

void SceneManager::FixedUpdate(const float fixedTimeStep)
{
	for (auto& scene : m_Scenes)
	{
		scene->FixedUpdate(fixedTimeStep);
	}
}

void SceneManager::Update(float deltaTime)
{
	for(auto& scene : m_Scenes)
	{
		scene->Update(deltaTime);
	}
}

void SceneManager::Render()
{
	for (const auto& scene : m_Scenes)
	{
		scene->Render();
	}
}

Scene* SceneManager::GetCurrentScene() const
{
	// Will change in the future
	return m_Scenes[0].get();
}

Scene& SceneManager::CreateScene()
{
	m_Scenes.emplace_back(new Scene());
	return *m_Scenes.back();
}
