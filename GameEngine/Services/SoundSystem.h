#pragma once
#include <cstdint>
#include <string>

namespace ge
{
	using Sound_Id = uint8_t;

	// For two different channels
	enum class SoundCategory
	{
		SFX,
		Music
	};

	// Abstract Interface for SOUND
	class SoundSystem
	{
	public:
		SoundSystem() = default;
		virtual ~SoundSystem() = default;
		SoundSystem(const SoundSystem& other) = delete;
		SoundSystem(SoundSystem&& other) = delete;
		SoundSystem& operator=(const SoundSystem& other) = delete;
		SoundSystem& operator=(SoundSystem&& other) = delete;
		
		virtual void Play(Sound_Id id, float volume, SoundCategory category = SoundCategory::SFX) = 0;
		virtual void RegisterSound(ge::Sound_Id id, const std::string& fileName) = 0;

		virtual void Stop(Sound_Id id) = 0;
		virtual void StopAll() = 0;
		virtual void SetMuted(bool muted) = 0;
		virtual bool IsMuted() const = 0;

	private:
	};
}