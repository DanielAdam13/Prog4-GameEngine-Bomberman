#pragma once
#include "Singleton.h"
#include "Texture2D.h" // needed for map
#include "Font.h" // needed for map
#include "SpriteSheet.h" // needed for map

#include <utility>
#include <filesystem>
#include <string>
#include <memory>
#include <map>
#include <string>
#include <cstdint>

namespace ge
{
	class ResourceManager final : public Singleton<ResourceManager>
	{
	public:
		void Init(const std::filesystem::path& data);
		Texture2D* LoadTexture(const std::string& file);
		Font* LoadFont(const std::string& file, uint8_t size);
		SpriteSheet* LoadSpriteSheet(const std::string& filePath, int columns, int rows);

		std::string GetFullPath(std::string&& relativePath) const noexcept;

	private:
		// Resources folder path
		std::filesystem::path m_DataPath;

		void UnloadUnusedResources();

		std::map<std::string, std::unique_ptr<Texture2D>> m_LoadedTextures;
		std::map<std::pair<std::string, uint8_t>, std::unique_ptr<Font>> m_LoadedFonts;
		std::map<std::string, std::unique_ptr<SpriteSheet>> m_LoadedSpriteSheets;

	};
}
