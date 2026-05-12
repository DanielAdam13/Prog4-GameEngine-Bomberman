#include "LevelBuilder.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Colliders.h"
#include "Scene.h"

#include <utility>

void bombGame::levelBuilder::BuildStaticGeometry(ge::Scene& scene, const levelLoader::LevelLayout& layout,
	const glm::vec3& buildTopLeftPos, const float tileSize)
{
	for (int row{ 0 }; row < layout.height; ++row)
	{
		for (int col{ 0 }; col < layout.width; ++col)
		{
			levelLoader::TileType currentTileType{ layout.At(col, row) };
			if (currentTileType == levelLoader::TileType::Wall)
			{
				const int currentIndex{ row * layout.width + col };
				auto wallGameObject = std::make_unique<ge::GameObject>("GO_Wall" + std::to_string(currentIndex));
				const glm::vec3 wallPos{ col * tileSize, row * tileSize, 0.f };
				wallGameObject->GetComponent<ge::Transform>()->SetLocalPosition(buildTopLeftPos + wallPos);
				auto collider{ wallGameObject->AddComponent<ge::BoxCollider>(wallGameObject.get(), glm::vec2{ tileSize,tileSize }) };
				collider->AssignTag("Wall");

				scene.Add(std::move(wallGameObject));
			}
		}
	}
}
