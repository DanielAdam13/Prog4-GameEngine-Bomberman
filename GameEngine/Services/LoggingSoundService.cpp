#include "LoggingSoundService.h"
#include "Services/NullSoundSystem.h"

#include <iostream>

ge::LoggingSoundSystem::LoggingSoundSystem(std::unique_ptr<SoundSystem>&& soundSys)
	:m_RealSoundSystem{ std::make_unique<NullSoundSystem>() }
{
	if (soundSys != nullptr)
		m_RealSoundSystem = std::move(soundSys);
}

void ge::LoggingSoundSystem::Play(const Sound_Id soundId, const float volume)
{
	m_RealSoundSystem->Play(soundId, volume);
	std::cout << "Playing " << soundId << " at volume " << volume << "\n";
}
