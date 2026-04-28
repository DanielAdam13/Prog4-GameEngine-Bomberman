#include "SDLSoundSystem.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <stdexcept>
#include <unordered_map>
#include <algorithm>
#include <vector>

namespace bombGame
{
	class SDLSoundSysImpl final
	{
	public:
		SDLSoundSysImpl();
		~SDLSoundSysImpl();

		void Play(const ge::Sound_Id soundId, const float volume);
		void RegisterSound(ge::Sound_Id id, const std::string& fileName);

	private:
		// Loads the file for ID. Returns the loaded audio. If already cached -> return the cache
		MIX_Audio* LoadAndCache(ge::Sound_Id soundId, const std::string& fileName);

		MIX_Mixer* m_Mixer{ nullptr };

		std::unordered_map<ge::Sound_Id, MIX_Audio*> m_LoadedAudios{};
		std::unordered_map<ge::Sound_Id, std::string> m_Filenames{};

		static constexpr size_t TrackPoolSize{ 16 };
		std::vector<MIX_Track*> m_TrackPool{};
		size_t m_NextTrack{ 0 };
	};

	SDLSoundSysImpl::SDLSoundSysImpl()
	{
		if (!MIX_Init())
		{
			throw std::runtime_error(std::string("MIX_Init failed: ") + SDL_GetError());
		}

		m_Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
		if(!m_Mixer)
			throw std::runtime_error(std::string("MIX_CreateMixerDevice failed: ") + SDL_GetError());

		// Initialize track pool!
		m_TrackPool.reserve(TrackPoolSize);
		for (size_t i{}; i < TrackPoolSize; ++i)
		{
			MIX_Track* track{ MIX_CreateTrack(m_Mixer) };
			if(!track)
				throw std::runtime_error(std::string("MIX_CreateTrack failed: ") + SDL_GetError());

			m_TrackPool.push_back(track);
		}
	}

	SDLSoundSysImpl::~SDLSoundSysImpl()
	{
		// Destroy track pool:
		for (auto* track : m_TrackPool)
		{
			MIX_DestroyTrack(track);
		}
		m_TrackPool.clear();

		// Destroy audio container:
		for (auto& [id, audio] : m_LoadedAudios)
		{
			MIX_DestroyAudio(audio);
		}
		m_LoadedAudios.clear();

		if (m_Mixer)
			MIX_DestroyMixer(m_Mixer);

		MIX_Quit();
	}
	void SDLSoundSysImpl::Play(const ge::Sound_Id soundId, const float volume)
	{
		// Already loaded:
		const auto it{ m_LoadedAudios.find(soundId) };
		MIX_Audio* audio{ (it != m_LoadedAudios.end()) ? it->second : nullptr };

		// If doesn't exist -> LOAD by cached filename
		if(!audio)
		{
			const auto fileIt{ m_Filenames.find(soundId) };

			if (fileIt == m_Filenames.end())
			{
				// Filename was never registered...
				return;
			}

			// And load
			audio = LoadAndCache(soundId, fileIt->second);
			if (!audio)
				return;
		}

		// Get track from track pool
		MIX_Track* track{ m_TrackPool[m_NextTrack] };
		// and update next track
		m_NextTrack = (m_NextTrack + 1) % m_TrackPool.size();

		const float clampedVolume{ std::clamp(volume, 0.f, 1.f) };
		MIX_SetTrackAudio(track, audio);
		MIX_SetTrackGain(track, clampedVolume);
		MIX_PlayTrack(track, 0); // 0 - default: play once

	}

	void SDLSoundSysImpl::RegisterSound(ge::Sound_Id id, const std::string& fileName)
	{
		// If a sound was already registered under this id, destroy it
		if (auto it{ m_LoadedAudios.find(id) }; it != m_LoadedAudios.end())
		{
			MIX_DestroyAudio(it->second);
			m_LoadedAudios.erase(it);
		}

		m_Filenames[id] = fileName;
		LoadAndCache(id, fileName);
	}

	MIX_Audio* SDLSoundSysImpl::LoadAndCache(ge::Sound_Id soundId, const std::string& fileName)
	{
		// If already existing, return cache
		if (auto it{ m_LoadedAudios.find(soundId) }; it != m_LoadedAudios.end())
		{
			return it->second;
		}

		// Else, LOAD it
		MIX_Audio* audio{ MIX_LoadAudio(m_Mixer, fileName.c_str(), true) };
		if(!audio)
			return nullptr;

		// And then cache it:
		m_LoadedAudios.emplace(soundId, audio);
		return audio;
	}
}

bombGame::SDLSoundSystem::SDLSoundSystem()
	:m_Impl{ std::make_unique<SDLSoundSysImpl>() }
{
}
void bombGame::SDLSoundSystem::Play(const ge::Sound_Id soundId, const float volume)
{
	m_Impl->Play(soundId, volume);
}

void bombGame::SDLSoundSystem::RegisterSound(ge::Sound_Id id, const std::string& fileName)
{
	m_Impl->RegisterSound(id, fileName);
}
