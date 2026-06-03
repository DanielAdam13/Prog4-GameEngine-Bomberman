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
        { balloomSheet, 60.f, 1, 150.f, 0.f };
    g_EnemyArchetypes[static_cast<size_t>(EnemyType::Onil)] =
        { onilSheet, 100.f, 1, 200.f, 0.4f };
    g_EnemyArchetypes[static_cast<size_t>(EnemyType::Dall)] =
        { dallSheet, 120.f, 1, 180.f, 0.3f };
    g_EnemyArchetypes[static_cast<size_t>(EnemyType::Minvo)] =
        { minvoSheet, 80.f, 1, 250.f, 0.7f };
}
