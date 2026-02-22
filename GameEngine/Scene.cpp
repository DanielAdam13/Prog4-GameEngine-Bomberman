#include <algorithm>
#include "Scene.h"

using namespace ge;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	m_Objects.push_back(std::move(object));
}

void Scene::Remove(const GameObject& object)
{
	m_Objects.erase(
		std::remove_if(
			m_Objects.begin(),
			m_Objects.end(),
			[&object](const auto& ptr) { return ptr.get() == &object; }
		),
		m_Objects.end()
	);
}

void Scene::RemoveAll()
{
	m_Objects.clear();
}

void Scene::FixedUpdate(float fixedTimeStep)
{
	for (auto& object : m_Objects)
	{
		if (object && !object->MarkedForDeletion())
			object->FixedUpdate(fixedTimeStep);
	}

	CleanupDestroyedGameObjects();
}

void Scene::Update(float deltaTime)
{
	for (auto& object : m_Objects)
	{
		if (object && !object->MarkedForDeletion())
			object->Update(deltaTime);
	}

	CleanupDestroyedGameObjects();
}

void Scene::Render() const
{
	for (const auto& object : m_Objects)
	{
		if (object && !object->MarkedForDeletion())
			object->Render();
	}
}

GameObject* ge::Scene::FindObjectByName(const std::string& goName) const
{
	auto objId{ std::find_if(m_Objects.begin(), m_Objects.end(), [&goName](const std::unique_ptr<GameObject>& obj)
		{
			return obj && !obj->MarkedForDeletion() && // Checks for lifetime availability
				obj->GetName() == goName; // Finally, check for the name
		}) };

	if (objId != m_Objects.end())
	{
		return objId->get();
	}

	return nullptr;
}

GameObject* ge::Scene::FindObjectByID(const size_t index) const
{
	if (index >= m_Objects.size())
		return nullptr;

	const auto& obj = m_Objects[index];

	if (!obj || obj->MarkedForDeletion()) // Checks for lifetime availability
		return nullptr;

	return obj.get();
}

void Scene::RemoveObjectByName(const std::string& goName)
{
	auto it = std::find_if(
		m_Objects.begin(),
		m_Objects.end(),
		[&goName](const std::unique_ptr<GameObject>& obj)
		{
			return obj && !obj->MarkedForDeletion() &&
				obj->GetName() == goName;
		});

	if (it != m_Objects.end())
	{
		(*it)->MarkForDeletion(); // MARK
	}
}

void Scene::RemoveObjectByID(const unsigned int index)
{
	if (index >= m_Objects.size())
		return;

	auto& obj{ m_Objects[index] };

	if (obj && !obj->MarkedForDeletion())
		obj->MarkForDeletion(); // MARK
}

void Scene::CleanupDestroyedGameObjects()
{
	m_Objects.erase(std::remove_if(m_Objects.begin(), m_Objects.end(),
		[](const std::unique_ptr<GameObject>& go)
		{
			return !go || go->MarkedForDeletion(); // Remove if marked or nullptr
		}),
		m_Objects.end());
}
