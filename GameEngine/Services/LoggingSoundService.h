#pragma once
#include "Services/SoundSystem.h"

#include <memory>

namespace ge
{
	// "Decorator" class which logs which sound info AFTER Real Sound System is played
	class LoggingSoundSystem final : public SoundSystem
	{
	public:
		LoggingSoundSystem(std::unique_ptr<SoundSystem>&& soundSys);
		virtual ~LoggingSoundSystem() override = default;

		virtual void Play(const Sound_Id soundId, const float volume) override;
		virtual void RegisterSound(ge::Sound_Id id, const std::string& fileName) override;

	private:
		std::unique_ptr<SoundSystem> m_RealSoundSystem{};
	};
}