#pragma once
#include "Services/SoundSystem.h"

#include <memory>
#include <string>

namespace ge
{
	// "Decorator" class which logs which sound info AFTER Real Sound System is played
	class LoggingSoundSystem final : public SoundSystem
	{
	public:
		LoggingSoundSystem(std::unique_ptr<SoundSystem>&& soundSys);
		virtual ~LoggingSoundSystem() override;
		LoggingSoundSystem(const LoggingSoundSystem& other) = delete;
		LoggingSoundSystem(LoggingSoundSystem&& other) = delete;
		LoggingSoundSystem& operator=(const LoggingSoundSystem& other) = delete;
		LoggingSoundSystem& operator=(LoggingSoundSystem&& other) = delete;

		virtual void Play(const Sound_Id soundId, const float volume, SoundCategory category = SoundCategory::SFX) override;
		virtual void RegisterSound(ge::Sound_Id id, const std::string& fileName) override;
		virtual void Stop(Sound_Id id) override;
		virtual void StopAll() override;
		virtual void SetMuted(bool muted) override;
		virtual bool IsMuted() const override;

	private:
		std::unique_ptr<SoundSystem> m_RealSoundSystem{};
	};
}