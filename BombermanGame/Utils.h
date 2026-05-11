#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <functional>

namespace ge
{
	class GameObject;
	class Texture2D;
}

namespace bombGame
{
	// Creates the reusable Bomb Game Object and returns it as a unique_ptr
	std::unique_ptr<ge::GameObject> CreateBomb(const glm::vec3& position, ge::Texture2D* texture, float explosionTimer);
}