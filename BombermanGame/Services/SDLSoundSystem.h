#pragma once
#include "Services/SoundSystem.h"

#include <memory>

namespace bombGame
{
	class SDLSoundSysImpl;

	class SDLSoundSystem : public ge::SoundSystem
	{
	public:
		SDLSoundSystem();
		virtual ~SDLSoundSystem() override = default;
		SDLSoundSystem(const SDLSoundSystem& other) = delete;
		SDLSoundSystem(SDLSoundSystem&& other) = delete;
		SDLSoundSystem& operator=(const SDLSoundSystem& other) = delete;
		SDLSoundSystem& operator=(SDLSoundSystem&& other) = delete;

		virtual void Play(const ge::Sound_Id soundId, const float volume) override;
		virtual void RegisterSound(ge::Sound_Id id, const std::string& fileName) override;

	private:
		std::unique_ptr<SDLSoundSysImpl> m_Impl{};
	};
}