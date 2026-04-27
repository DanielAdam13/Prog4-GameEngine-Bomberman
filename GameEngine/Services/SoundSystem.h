#pragma once
#include <cstdint>

namespace ge
{
	using Sound_Id = uint8_t;

	// Abstract Interface deriving for SOUND
	class SoundSystem
	{
	public:
		SoundSystem() = default;
		virtual ~SoundSystem() = default;

		virtual void Play(const Sound_Id id, const float volume) = 0;

	private:
	};
}