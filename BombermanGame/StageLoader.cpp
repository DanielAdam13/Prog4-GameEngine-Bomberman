#include "StageLoader.h"

#include <cassert>

using namespace bombGame;

namespace
{
    static const std::vector<stageLoader::StageDescriptor> GameStages{ { "mainLevel1.txt", 6, { {EnemyType::Balloom, 6} }, PowerupType::FireUp },
            { "mainLevel1.txt", 5, {{EnemyType::Balloom, 6}, {EnemyType::Onil, 3}}, PowerupType::BombUp },
            { "mainLevel1.txt", 4, {{EnemyType::Balloom, 4}, {EnemyType::Onil, 2}, {EnemyType::Minvo, 2}, {EnemyType::Dall, 2}}, PowerupType::RemoteDetonate } };
}

const stageLoader::StageDescriptor& bombGame::stageLoader::Load(size_t stageIndex)
{
    assert(stageIndex < GameStages.size() && "Stage index out of range");
    return GameStages[stageIndex];
}

stageLoader::StageDescriptor bombGame::stageLoader::Load(const std::string&)
{
    // Not needed for my case in my opinion...
    return StageDescriptor();
}

int bombGame::stageLoader::GetStageCount() noexcept
{
    return static_cast<int>(GameStages.size());
}
