#include "LevelLoader.h"

#include <fstream>
#include <stdexcept>

using namespace bombGame;

levelLoader::LevelLayout levelLoader::Load(const std::string& path)
{
    std::ifstream file{ path };
    if (!file.is_open())
        throw std::runtime_error("LevelLoader: could not open " + path);

    LevelLayout layout{};
    layout.width = 0;
    layout.height = 0;
    layout.player1SpawnPoint = { -1, -1 };
    layout.player2SpawnPoint = { -1, -1 };

    std::string line;
    int row{ 0 };

    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line.empty())
            continue;

        if (layout.width == 0)
        {
            layout.width = static_cast<int>(line.size());
        }
        else if(static_cast<int>(line.size()) != layout.width)
        {
            throw std::runtime_error("LevelLoader: row " + std::to_string(row)
                + " has width " + std::to_string(line.size())
                + ", expected " + std::to_string(layout.width));
        }

        for (int col{ 0 }; col < layout.width; ++col)
        {
            const char c{ line[col] };
            switch (c)
            {
            case '.':
                layout.tilesContainer.push_back(TileType::Empty);
                break;
            case '#':
                layout.tilesContainer.push_back(TileType::Wall);
                break;
            case '1':
                layout.tilesContainer.push_back(TileType::Empty);
                layout.player1SpawnPoint = { col, row };
                break;
            case '2':
                layout.tilesContainer.push_back(TileType::Empty);
                layout.player2SpawnPoint = { col, row };
                break;
            default:
                throw std::runtime_error(std::string("LevelLoader: unknown tile '") + c);
            }
        }

        ++row;
    }

    layout.height = row;

    if(layout.width == 0 || layout.width == 0)
        throw std::runtime_error("LevelLoader: empty level file " + path);

    return layout;
}
