#include "LevelGrid.h"

bombGame::LevelGrid::LevelGrid(const levelLoader::LevelLayout& layout, const glm::vec3& buildTopLeftPos, const float tileSize)
	:m_LevelLayout{ layout },
	m_LevelTopLeftPos{ buildTopLeftPos },
	m_TileSize{ tileSize },
	m_DynamicWalls(layout.width * layout.height, nullptr), // Container size is known during runtime
	m_CurrentlyActiveBombs(layout.width * layout.height, nullptr) // Container size is known during runtime
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

	return GridTile{ tileRect, index, tileType, col, row };
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

	return GridTile{ tileRect, index, tileType, col, row };
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

	return glm::vec2{ m_LevelTopLeftPos.x + col * m_TileSize + m_TileSize * 0.5f,
		m_LevelTopLeftPos.y + row * m_TileSize + m_TileSize * 0.5f };
}

std::optional<glm::vec2> bombGame::LevelGrid::GetMidGridTilePointByCoord(int col, int row) const noexcept
{
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

void bombGame::LevelGrid::MarkExitLocationAt(int col, int row)
{
	m_ExitCoords = { col, row };
}

std::pair<int, int> bombGame::LevelGrid::GetExitLocation() const noexcept
{
	return m_ExitCoords;
}

bool bombGame::LevelGrid::IsExitTile(int col, int row) const noexcept
{
	return m_ExitCoords.first == col && m_ExitCoords.second == row;
}

void bombGame::LevelGrid::RegisterExit(ge::GameObject* go)
{
	m_ExitGO = go;
}

ge::GameObject* bombGame::LevelGrid::GetExitObject() const noexcept
{
	return m_ExitGO;
}

void bombGame::LevelGrid::RegisterBombAt(int col, int row, ge::GameObject* go)
{
	m_CurrentlyActiveBombs[ToIndex(col, row)] = go;
}

ge::GameObject* bombGame::LevelGrid::GetBombAt(int col, int row) const noexcept
{
	return m_CurrentlyActiveBombs[ToIndex(col, row)];
}

void bombGame::LevelGrid::ClearBombAt(int col, int row)
{
	m_CurrentlyActiveBombs[ToIndex(col, row)] = nullptr;
}

int bombGame::LevelGrid::ToIndex(int col, int row) const noexcept
{
	return row * m_LevelLayout.width + col;
}

bool bombGame::LevelGrid::IsBlocked(const GridTile& tile) const noexcept
{
	if (tile.gridTileType == levelLoader::TileType::Wall)
		return true;
	if (this->GetBreakableWallAt(tile.col, tile.row))
		return true;
	if (this->GetBombAt(tile.col, tile.row))
		return true;
	return false;
}