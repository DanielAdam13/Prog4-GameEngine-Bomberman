#pragma once
#include "Services/SoundSystem.h"

namespace ge
{
	// Empty/Null interface derived class
	class NullSoundSystem final : public SoundSystem
	{
	public:
		virtual ~NullSoundSystem() override = default;

		// Empty/Null Play implementation
		virtual void Play(Sound_Id, float, SoundCategory) override {};
		virtual void RegisterSound(ge::Sound_Id, const std::string&) override {};

		virtual void Stop(Sound_Id) override {}
		virtual void StopAll() override {}
		virtual void SetMuted(bool) override {}
		virtual bool IsMuted() const override { return false; }

	private:

	};
}