#include "LevelBuilder.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Colliders.h"
#include "Components/Image.h"
#include "Texture2D.h"
#include "Scene.h"
#include "Structs.h"

#include <utility>
#include <memory>
#include <random>
#include <glm/glm.hpp>
#include <optional>

bombGame::LevelGrid::LevelGrid(const levelLoader::LevelLayout& layout, const glm::vec3& buildTopLeftPos, const float tileSize)
	:m_LevelLayout{ layout },
	m_LevelTopLeftPos{ buildTopLeftPos },
	m_TileSize{ tileSize },
	m_DynamicWalls(layout.width* layout.height, nullptr)
{
}

std::optional<bombGame::GridTile> bombGame::LevelGrid::GetGridTileAt(const glm::vec3& pos) const noexcept
{
	const float localX{ pos.x - m_LevelTopLeftPos.x };
	const float localY{ pos.y - m_LevelTopLeftPos.y };

	// Out of bounds
	if (localX < 0.f || localY < 0.f)
		return std::nullopt;

	const int col{ static_cast<int>(localX / m_TileSize) };
	const int row{ static_cast<int>(localY / m_TileSize) };

	// Out of bound
	if (col >= m_LevelLayout.width || row >= m_LevelLayout.height)
		return std::nullopt;

	const int index{ row * m_LevelLayout.width + col };
	const auto tileType{ m_LevelLayout.At(col, row) };

	const ge::structs::Rect tileRect{
		glm::vec2{m_LevelTopLeftPos.x + col * m_TileSize, m_LevelTopLeftPos.y + row * m_TileSize },
		glm::vec2{m_TileSize, m_TileSize} };

	return GridTile{ tileRect, index, tileType };
}

std::optional<bombGame::GridTile> bombGame::LevelGrid::GetGridTileByCoord(int col, int row) const noexcept
{
	if (col < 0 || row < 0 || col >= m_LevelLayout.width || row >= m_LevelLayout.height)
		return std::nullopt;

	const int index{ row * m_LevelLayout.width + col };
	const auto tileType{ m_LevelLayout.At(col, row) };
	const ge::structs::Rect tileRect{
		glm::vec2{m_LevelTopLeftPos.x + col * m_TileSize, m_LevelTopLeftPos.y + row * m_TileSize },
		glm::vec2{m_TileSize, m_TileSize} };

	return GridTile{ tileRect, index, tileType };
}

std::optional<glm::vec2> bombGame::LevelGrid::GetMidGridTilePointAt(const glm::vec3& worldPos) const noexcept
{
	const float localX{ worldPos.x - m_LevelTopLeftPos.x };
	const float localY{ worldPos.y - m_LevelTopLeftPos.y };

	// Out of bounds
	if (localX <= 0.f || localY <= 0.f)
		return std::nullopt;

	const int col{ static_cast<int>(localX / m_TileSize) };
	const int row{ static_cast<int>(localY / m_TileSize) };

	// Out of bound
	if (col >= m_LevelLayout.width || row >= m_LevelLayout.height)
		return std::nullopt;

	return glm::vec2{ m_LevelTopLeftPos.x + col * m_TileSize + m_TileSize / 2.f, 
		m_LevelTopLeftPos.y + row * m_TileSize + m_TileSize / 2.f };
}

glm::vec3 bombGame::LevelGrid::GetLevelTopLeft() const noexcept
{
	return m_LevelTopLeftPos;
}

bombGame::levelLoader::LevelLayout bombGame::LevelGrid::GetLevelLayout() const noexcept
{
	return m_LevelLayout;
}

float bombGame::LevelGrid::GetTileSize() const noexcept
{
	return m_TileSize;
}

void bombGame::LevelGrid::RegisterBreakableAt(int col, int row, ge::GameObject* go)
{
	m_DynamicWalls[ToIndex(col, row)] = go;
}

ge::GameObject* bombGame::LevelGrid::GetBreakableWallAt(int col, int row) const noexcept
{
	return m_DynamicWalls[ToIndex(col, row)];
}

void bombGame::LevelGrid::ClearBreakableAt(int col, int row)
{
	m_DynamicWalls[ToIndex(col, row)] = nullptr;
}

int bombGame::LevelGrid::ToIndex(int col, int row) const noexcept
{
	return row * m_LevelLayout.width + col;
}

void bombGame::levelBuilder::BuildStaticGeometry(ge::Scene& scene, const LevelGrid& grid)
{
	for (int row{ 0 }; row < grid.GetLevelLayout().height; ++row)
	{
		for (int col{ 0 }; col < grid.GetLevelLayout().width; ++col)
		{
			levelLoader::TileType currentTileType{ grid.GetLevelLayout().At(col, row)};
			if (currentTileType == levelLoader::TileType::Wall)
			{
				const int currentIndex{ grid.ToIndex(col, row) };
				auto wallGameObject = std::make_unique<ge::GameObject>("GO_Wall" + std::to_string(currentIndex));
				const glm::vec3 wallPos{ col * grid.GetTileSize() , row * grid.GetTileSize(), 0.f};
				wallGameObject->GetComponent<ge::Transform>()->SetLocalPosition(grid.GetLevelTopLeft() + wallPos);
				auto collider{ wallGameObject->AddComponent<ge::BoxCollider>(wallGameObject.get(),
					glm::vec2{ grid.GetTileSize(), grid.GetTileSize() }, false) };
				collider->AssignTag("Wall");

				scene.Add(std::move(wallGameObject));
			}
		}
	}
}

void bombGame::levelBuilder::GenerateDynamicObjects(ge::Scene& scene, LevelGrid& grid, 
	ge::Texture2D* breakableWallTexture, int breakableWallRandomnessIndex)
{
	for (int row{ 0 }; row < grid.GetLevelLayout().height; ++row)
	{
		for (int col{ 0 }; col < grid.GetLevelLayout().width; ++col)
		{
			levelLoader::TileType currentTileType{ grid.GetLevelLayout().At(col, row)};

			// 1 out of [breakableWallRandomnessIndex] chance to spawn a breakable wall on an Empty spot
			if (currentTileType == levelLoader::TileType::Empty)
			{
				static std::mt19937 gen{ std::random_device{}() };
				static std::uniform_int_distribution<size_t> dist(0, breakableWallRandomnessIndex);

				if (dist(gen) == 0)
				{
					const int currentIndex{ grid.ToIndex(col, row) };
					auto breakableWallGO = std::make_unique<ge::GameObject>("GO_BreakableWall" + std::to_string(currentIndex));
					const glm::vec3 brWallPos{ col * grid.GetTileSize(), row * grid.GetTileSize(), 0.f};
					auto breakTr{ breakableWallGO->GetComponent<ge::Transform>() };
					breakTr->SetLocalPosition(grid.GetLevelTopLeft() + brWallPos);
					breakTr->SetLocalScale(3.5f, 3.5f, 1.f);

					auto brWallImage{ breakableWallGO->AddComponent<ge::Image>(breakableWallGO.get()) };
					brWallImage->SetTexture(breakableWallTexture);
					auto collider{ breakableWallGO->AddComponent<ge::BoxCollider>(breakableWallGO.get(),
						glm::vec2{ grid.GetTileSize(), grid.GetTileSize() }, true) };
					collider->AssignTag("BreakableWall");

					grid.RegisterBreakableAt(col, row, breakableWallGO.get());

					scene.Add(std::move(breakableWallGO));
				}
			}
		}
	}
}
