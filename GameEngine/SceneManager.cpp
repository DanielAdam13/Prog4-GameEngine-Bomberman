#include "SceneManager.h"
#include "Scene.h"
#include "EngineEvents.h"

#include <string>
#include <stdexcept>
#include <memory>
#include <cassert>
#include <utility>

using namespace ge;

void SceneManager::FixedUpdate(const float fixedTimeStep)
{
	if (m_CurrentActiveScene)
		m_CurrentActiveScene->FixedUpdate(fixedTimeStep);
}

void SceneManager::Update(float deltaTime)
{
	if (m_CurrentActiveScene)
		m_CurrentActiveScene->Update(deltaTime);
}

void SceneManager::Render()
{
	if (m_CurrentActiveScene)
		m_CurrentActiveScene->Render();
}

Scene* SceneManager::GetCurrentActiveScene() const
{
	return m_CurrentActiveScene;
}

void ge::SceneManager::SwitchToSceneWithName(const std::string& sceneName)
{
	auto it{ m_ScenesContainer.find(sceneName) };

	if (it == m_ScenesContainer.end())
		throw std::runtime_error("Scene '" + sceneName + "' not found, cannot switch");
	
	m_CurrentActiveScene = it->second.get();
}

bool ge::SceneManager::RemoveSceneWithName(const std::string& sceneName)
{
	auto it{ m_ScenesContainer.find(sceneName) };
	if (it == m_ScenesContainer.end())
	{
		assert("Trying to remove scene with nonexistent name");
		return false;
	}

	if (m_CurrentActiveScene == it->second.get())
		m_CurrentActiveScene = nullptr;

	m_ScenesContainer.erase(it);
	return true;
}

Scene& SceneManager::CreateScene(const std::string& sceneName)
{
	auto [it, inseted] = m_ScenesContainer.try_emplace(sceneName, new Scene(sceneName));
	
	if(!inseted)
		throw std::runtime_error("Scene with name '" + sceneName + "' already exists");

	if (!m_CurrentActiveScene)
		m_CurrentActiveScene = it->second.get();

	return *it->second;
}
