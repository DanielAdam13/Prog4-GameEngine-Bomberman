#include "ServiceLocator.h"
#include "NullSoundSystem.h"

#include <utility>

std::unique_ptr<ge::SoundSystem> ge::ServiceLocator::SoundSystem_Instance{};
std::unique_ptr<ge::InputManager> ge::ServiceLocator::InputManager_Instance{};

void ge::ServiceLocator::RegisterInputManager(std::unique_ptr<InputManager>&& inMan)
{
	InputManager_Instance = std::move(inMan);
}

ge::InputManager& ge::ServiceLocator::GetInputManager()
{
	return *InputManager_Instance;
}

void ge::ServiceLocator::RegisterSoundSystem(std::unique_ptr<SoundSystem>&& soundSys)
{
	SoundSystem_Instance = 
		(soundSys == nullptr) ? std::make_unique<ge::NullSoundSystem>() : std::move(soundSys);
}

ge::SoundSystem& ge::ServiceLocator::GetSoundSystem()
{
	return *SoundSystem_Instance;
}
