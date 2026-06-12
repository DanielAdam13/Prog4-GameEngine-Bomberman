#include "HighScoreList.h"

#include <algorithm>
#include <fstream>
#include <sstream>

bool bombGame::HighScoreList::TryInsert(const HighScoreEntry& entry)
{
    auto pos{ std::find_if(m_Entries.begin(), m_Entries.end(),
        [&](const HighScoreEntry& e) -> bool { return entry.score > e.score; }) };

    // Did not qualify -> early out
    if (pos == m_Entries.end() && m_Entries.size() >= MaxEntries)
    {
        return false;
    }

    m_Entries.insert(pos, entry);
    if (m_Entries.size() > MaxEntries)
        m_Entries.resize(MaxEntries);

    return true; // Succeeded
}

void bombGame::HighScoreList::LoadFromFile(const std::string& path)
{
    m_Entries.clear();
    std::ifstream file(path);
    if (!file)
        return;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::istringstream lineStream(line);
        HighScoreEntry entry;

        if (std::getline(lineStream, entry.name, ',') &&
            lineStream >> entry.score)
        {
            m_Entries.push_back(entry);
            if (m_Entries.size() >= MaxEntries) 
                break;
        }
    }
}

void bombGame::HighScoreList::SaveToFile(const std::string& path) const
{
    std::ofstream file(path);
    if (!file)
        return;

    for (const auto& entry : m_Entries)
    {
        file << entry.name << ',' << entry.score << '\n';
    }
}
