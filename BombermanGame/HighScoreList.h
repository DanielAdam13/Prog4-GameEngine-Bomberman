#pragma once

#include <string>
#include <vector>

namespace bombGame
{
	struct HighScoreEntry
	{
		std::string name;
		int score;
	};

	// Wrapper of a std::vector that also loads/saves to file
	class HighScoreList final
	{
	public:
		static constexpr size_t MaxEntries{ 5 };

		bool TryInsert(const HighScoreEntry& entry);

		const std::vector<HighScoreEntry>& GetEntries() const noexcept { return m_Entries; }

		// Loaded in BombermanGame::Load
		void LoadFromFile(const std::string& path);
		// Saved in VictoryState and LossState's OnEnter
		void SaveToFile(const std::string& path) const;

	private:
		std::vector<HighScoreEntry> m_Entries;

	};
}