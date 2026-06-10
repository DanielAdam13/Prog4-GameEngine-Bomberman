#include "EnemyArchetypes.h"

#include <array>

// Private, header hidden ARRAY of archetypes
namespace
{
    std::array<bombGame::EnemyArchetype, 4> g_EnemyArchetypes;
}

const bombGame::EnemyArchetype& bombGame::enemyArchetypes::Get(EnemyType type)
{
    return g_EnemyArchetypes[static_cast<size_t>(type)];
}

void bombGame::enemyArchetypes::InitializeArchetypes(ge::SpriteSheet* balloomSheet, ge::SpriteSheet* onilSheet, 
    ge::SpriteSheet* dallSheet, ge::SpriteSheet* minvoSheet)
{
    g_EnemyArchetypes[static_cast<size_t>(EnemyType::Balloom)] =
        { balloomSheet, 60.f, 1, 130.f, 100 };
    g_EnemyArchetypes[static_cast<size_t>(EnemyType::Onil)] =
        { onilSheet, 120.f, 1, 180.f, 200 };
    g_EnemyArchetypes[static_cast<size_t>(EnemyType::Dall)] =
        { dallSheet, 60.f, 1, 180.f, 400 };
    g_EnemyArchetypes[static_cast<size_t>(EnemyType::Minvo)] =
        { minvoSheet, 120.f, 1, 230.f, 800 };
}
