#include "Scene.h"

#include <algorithm>
#include <utility>

using namespace ge;

Scene::Scene(const std::string& sceneName)
	:m_SceneName{sceneName}
{
}

void Scene::Add(std::unique_ptr<GameObject> object)
{
	// If we Add during runtime, the whole container will become invalid -> UB
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	m_PendingAdditions.push_back(std::move(object));
}

void Scene::Remove(const GameObject& object)
{
	m_GameObjects.erase(
		std::remove_if(
			m_GameObjects.begin(),
			m_GameObjects.end(),
			[&object](const auto& ptr) { return ptr.get() == &object; }
		),
		m_GameObjects.end()
	);
}

void Scene::RemoveAll()
{
	m_GameObjects.clear();
}

void Scene::FixedUpdate(float fixedTimeStep)
{
	for (auto& object : m_GameObjects)
	{
		if (object && !object->MarkedForDeletion())
			object->FixedUpdate(fixedTimeStep);
	}

	CleanupDestroyedGameObjects();
}

void Scene::Update(float deltaTime)
{
	for (auto& object : m_GameObjects)
	{
		if (object && !object->MarkedForDeletion())
			object->Update(deltaTime);
	}

	// After updating -> add new GO for next frame
	// !!
	// Add delayed, Pending Game Object so the container is not invalid(UB)
	for (auto& go : m_PendingAdditions) 
	{
		m_GameObjects.push_back(std::move(go));
	}
	m_PendingAdditions.clear();
	// !!

	for (auto& imgui : m_ImGuiInstances)
	{
		if(imgui)
			imgui->UpdateImGui(deltaTime);
	}

	CleanupDestroyedGameObjects();
}

void Scene::Render() const
{
	for (const auto& object : m_GameObjects)
	{
		if (object && !object->MarkedForDeletion())
			object->Render();
	}

	for (const auto& imgui : m_ImGuiInstances)
	{
		if(imgui)
			imgui->RenderImGui();
	}
}

GameObject* ge::Scene::FindObjectByName(const std::string& goName) const
{
	auto objId{ std::find_if(m_GameObjects.begin(), m_GameObjects.end(), [&goName](const std::unique_ptr<GameObject>& obj)
		{
			return obj && !obj->MarkedForDeletion() && // Checks for lifetime availability
				obj->GetName() == goName; // Finally, check for the name
		}) };

	if (objId != m_GameObjects.end())
	{
		return objId->get();
	}

	return nullptr;
}

GameObject* ge::Scene::FindObjectByID(const size_t index) const
{
	if (index >= m_GameObjects.size())
		return nullptr;

	const auto& obj = m_GameObjects[index];

	if (!obj || obj->MarkedForDeletion()) // Checks for lifetime availability
		return nullptr;

	return obj.get();
}

void Scene::RemoveObjectByName(const std::string& goName)
{
	auto it = std::find_if(
		m_GameObjects.begin(),
		m_GameObjects.end(),
		[&goName](const std::unique_ptr<GameObject>& obj)
		{
			return obj && !obj->MarkedForDeletion() &&
				obj->GetName() == goName;
		});

	if (it != m_GameObjects.end())
	{
		(*it)->MarkForDeletion(); // MARK
	}
}

void Scene::RemoveObjectByID(const unsigned int index)
{
	if (index >= m_GameObjects.size())
		return;

	auto& obj{ m_GameObjects[index] };

	if (obj && !obj->MarkedForDeletion())
		obj->MarkForDeletion(); // MARK
}

void Scene::AddImGuiScene(std::unique_ptr<ImGuiInstance> imguiScene)
{
	if (imguiScene)
		m_ImGuiInstances.emplace_back(std::move(imguiScene));
}

std::string ge::Scene::GetSceneName() const noexcept
{
	return m_SceneName;
}

void Scene::CleanupDestroyedGameObjects()
{
	m_GameObjects.erase(std::remove_if(m_GameObjects.begin(), m_GameObjects.end(),
		[](const auto& go)
		{
			return !go || go->MarkedForDeletion(); // Remove if marked or nullptr
		}),
		m_GameObjects.end());
}
