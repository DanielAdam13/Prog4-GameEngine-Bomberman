#pragma once
#include "Services/SoundSystem.h"

#include <memory>

namespace bombGame
{
	class SDLSoundSysImpl;

	class SDLSoundSystem final : public ge::SoundSystem
	{
	public:
		SDLSoundSystem();
		virtual ~SDLSoundSystem() override;
		SDLSoundSystem(const SDLSoundSystem& other) = delete;
		SDLSoundSystem(SDLSoundSystem&& other) = delete;
		SDLSoundSystem& operator=(const SDLSoundSystem& other) = delete;
		SDLSoundSystem& operator=(SDLSoundSystem&& other) = delete;

		virtual void Play(const ge::Sound_Id soundId, const float volume, ge::SoundCategory category = ge::SoundCategory::SFX) override;
		virtual void RegisterSound(ge::Sound_Id id, const std::string& fileName) override;

		virtual void Stop(ge::Sound_Id) {} // Empty, I don't need it
		virtual void StopAll();
		virtual void SetMuted(bool muted);
		virtual bool IsMuted() const;

	private:
		std::unique_ptr<SDLSoundSysImpl> m_Impl{};
	};
}