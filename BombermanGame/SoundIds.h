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
		constexpr ge::Sound_Id StepHorizontal{ 5 };
		constexpr ge::Sound_Id StepVertical{ 6 };
		constexpr ge::Sound_Id MainMenuOST{ 7 };
		constexpr ge::Sound_Id GameplayNormalOST{ 8 };
		constexpr ge::Sound_Id StageEnter{ 9 };
		constexpr ge::Sound_Id StageWon{ 10 };
		constexpr ge::Sound_Id StageLost{ 11 };
		constexpr ge::Sound_Id GameLost{ 12 };
	}
}