#include "PowerupArchetypes.h"

#include <array>

// Private, header hidden ARRAY of archetypes
namespace
{
    std::array<bombGame::PowerupArchetype, 3> g_PowerupArchetypes;
}

const bombGame::PowerupArchetype& bombGame::powerupArchetypes::Get(PowerupType type)
{
    return g_PowerupArchetypes[static_cast<size_t>(type)];
}

void bombGame::powerupArchetypes::InitializeArchetypes(ge::Texture2D* fireUpTexture, ge::Texture2D* bombUpTexture, ge::Texture2D* remoteDetonateTexture)
{
    g_PowerupArchetypes[static_cast<size_t>(PowerupType::FireUp)] =
    { fireUpTexture, 200 };
    g_PowerupArchetypes[static_cast<size_t>(PowerupType::BombUp)] =
    { bombUpTexture, 300 };
    g_PowerupArchetypes[static_cast<size_t>(PowerupType::RemoteDetonate)] =
    { remoteDetonateTexture, 400 };
}
