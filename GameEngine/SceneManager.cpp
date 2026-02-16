#include "SceneManager.h"
#include "Scene.h"

using namespace ge;

void SceneManager::Update()
{
	for(auto& scene : m_Scenes)
	{
		scene->Update();
	}
}

void SceneManager::Render()
{
	for (const auto& scene : m_Scenes)
	{
		scene->Render();
	}
}

Scene& SceneManager::CreateScene()
{
	m_Scenes.emplace_back(new Scene());
	return *m_Scenes.back();
}
