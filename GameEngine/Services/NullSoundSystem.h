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
		virtual void Play(const Sound_Id, const float) override {};
	private:

	};
}