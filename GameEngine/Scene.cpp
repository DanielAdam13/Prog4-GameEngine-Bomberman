#include <algorithm>
#include "Scene.h"

using namespace ge;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	m_objects.push_back(std::move(object));
}

void Scene::Remove(const GameObject& object)
{
	m_objects.erase(
		std::remove_if(
			m_objects.begin(),
			m_objects.end(),
			[&object](const auto& ptr) { return ptr.get() == &object; }
		),
		m_objects.end()
	);
}

void Scene::RemoveAll()
{
	m_objects.clear();
}

void Scene::Update()
{
	for(auto& object : m_objects)
	{
		object->Update();
	}
}

void Scene::Render() const
{
	for (const auto& object : m_objects)
	{
		object->Render();
	}
}

GameObject* ge::Scene::FindObjectByName(const std::string& goName) const
{
	auto objId{ std::find_if(m_objects.begin(), m_objects.end(), [&goName](const std::unique_ptr<GameObject>& obj) 
		{
			return obj->GetName() == goName;
		}) };

	if (objId != m_objects.end())
	{
		return objId->get();
	}
	
	return nullptr;
}

