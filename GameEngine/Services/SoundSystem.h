#pragma once
#include <cstdint>
#include <string>

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
		virtual void RegisterSound(ge::Sound_Id id, const std::string& fileName) = 0;

	private:
	};
}