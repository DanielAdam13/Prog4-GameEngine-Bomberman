#pragma once
#include "Services/SoundSystem.h"

namespace ge
{
	class SDLSoundSystem : public SoundSystem
	{
	public:
		SDLSoundSystem() = default;
		virtual ~SDLSoundSystem() override = default;

		virtual void Play(const Sound_Id soundId, const float volume) override;

	private:

	};
}