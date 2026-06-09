#include "LoggingSoundService.h"
#include "Services/NullSoundSystem.h"

#include <iostream>

ge::LoggingSoundSystem::LoggingSoundSystem(std::unique_ptr<SoundSystem>&& soundSys)
	:m_RealSoundSystem{ std::make_unique<NullSoundSystem>() }
{
	if (soundSys != nullptr)
		m_RealSoundSystem = std::move(soundSys);
}

ge::LoggingSoundSystem::~LoggingSoundSystem()
{
	m_RealSoundSystem.reset();
}

void ge::LoggingSoundSystem::Play(const Sound_Id soundId, const float volume, SoundCategory category)
{
	m_RealSoundSystem->Play(soundId, volume, category);
	std::cout << "Playing " << soundId << " at volume " << volume << "\n";
}

void ge::LoggingSoundSystem::RegisterSound(ge::Sound_Id id, const std::string& fileName)
{
	m_RealSoundSystem->RegisterSound(id, fileName);
	std::cout << "Registered sound with id: " << id << "\n";
}

void ge::LoggingSoundSystem::Stop(Sound_Id id)
{
	m_RealSoundSystem->Stop(id);
	std::cout << "Stopped sound with id: " << id << "\n";
}

void ge::LoggingSoundSystem::StopAll()
{
	m_RealSoundSystem->StopAll();
	std::cout << "Stopped all sounds \n";
}

void ge::LoggingSoundSystem::SetMuted(bool muted)
{
	m_RealSoundSystem->SetMuted(muted);
	std::cout << "Set muted to " << muted << "\n";
}

bool ge::LoggingSoundSystem::IsMuted() const
{
	return m_RealSoundSystem->IsMuted();
}
