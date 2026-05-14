#pragma once
#include "Services/SoundSystem.h"

namespace bombGame
{
	namespace SoundIds
	{
		constexpr ge::Sound_Id ExplosionBomb{ 0 };
		constexpr ge::Sound_Id LayBomb{ 1 };
		constexpr ge::Sound_Id BombermanDied{ 2 };
		constexpr ge::Sound_Id Pause{ 3 };
		constexpr ge::Sound_Id Powerup{ 4 };
		constexpr ge::Sound_Id Step_Horizontal{ 5 };
		constexpr ge::Sound_Id Step_Vertical{ 6 };
	}
}