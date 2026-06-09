#include "SDLSoundSystem.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <stdexcept>
#include <unordered_map>
#include <algorithm>
#include <vector>

#ifndef __EMSCRIPTEN__
#include <condition_variable>
#include <thread>
#endif

#include <queue>
#include <atomic>
#include <cassert>

namespace bombGame
{
	// Type passed to the queue executed from the worker thread
	struct PlayRequest
	{
		ge::Sound_Id soundId;
		float volume;
		ge::SoundCategory category;
	};


	class SDLSoundSysImpl final
	{
	public:
		SDLSoundSysImpl();
		~SDLSoundSysImpl();

		// Push the PlayRequest to the play queue
		void Play(ge::Sound_Id soundId, float volume, ge::SoundCategory category);

		// DANGER - Might cause a data race due to writing to m_FileNames while
		// the worker thread might be processing it
		void RegisterSound(ge::Sound_Id id, const std::string& fileName);

		void StopAll();
		void SetMuted(bool muted);
		bool IsMuted() const { return m_Muted; }

	private:
		// Loads the file for ID. Returns the loaded audio. If already cached -> return the cache
		MIX_Audio* LoadAndCache(ge::Sound_Id soundId, const std::string& fileName);

		MIX_Mixer* m_Mixer{ nullptr };

		std::atomic<bool> m_Muted{ false };

		std::unordered_map<ge::Sound_Id, MIX_Audio*> m_LoadedAudios{};
		std::unordered_map<ge::Sound_Id, std::string> m_FileNames{};

		static constexpr size_t SFXPoolSize{ 16 };
		static constexpr size_t MusicTrackSize{ 1 };
		std::vector<MIX_Track*> m_SFXPool{};
		std::vector<MIX_Track*> m_MusicTrackPool{};
		size_t m_NextSFXTrack{ 0 };

#ifndef __EMSCRIPTEN__
		// Threading:
		std::queue<PlayRequest> m_PlayQueue{};
		std::mutex m_AudioStateMutex{}; // Mutex needed for play, register, destructor, mute locks
		std::condition_variable m_Cv{}; // Condition variable to wait on for ExecutePlay()
		std::atomic<bool> m_ShouldQuit{ false }; // for when window is closed
		std::thread m_WorkerThread{}; // Thread for Load and Play()

		// Processes Queue by the workrer thread
		void WorkerLoop();
#endif // !__EMSCRIPTEN__

		// Actual sequential PlayTrack logic.
		// Branches depending on category - SFX/Music
		void ExecutePlay(const ge::Sound_Id soundId, const float volume, ge::SoundCategory category);
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

		// Initialize musicTr pool!
		m_SFXPool.reserve(SFXPoolSize);
		for (size_t i{}; i < SFXPoolSize; ++i)
		{
			MIX_Track* track{ MIX_CreateTrack(m_Mixer) };
			if(!track)
				throw std::runtime_error(std::string("MIX_CreateTrack failed: ") + SDL_GetError());

			m_SFXPool.push_back(track);
		}

		// Intialize music pool the same way
		m_MusicTrackPool.reserve(MusicTrackSize);
		for (size_t i{}; i < MusicTrackSize; ++i)
		{
			MIX_Track* track{ MIX_CreateTrack(m_Mixer) };
			if (!track)
				throw std::runtime_error(std::string("MIX_CreateTrack failed: ") + SDL_GetError());

			m_MusicTrackPool.push_back(track);
		}

#ifndef __EMSCRIPTEN__
		// Initialize worker thread with its dedicated method
		m_WorkerThread = std::thread(&SDLSoundSysImpl::WorkerLoop, this);
#endif
	}

	SDLSoundSysImpl::~SDLSoundSysImpl()
	{
#ifndef __EMSCRIPTEN__
		{
			std::lock_guard<std::mutex> lock(m_AudioStateMutex);
			m_ShouldQuit.store(true);
		} // lock goes out of scope
		m_Cv.notify_one();
		if (m_WorkerThread.joinable())
			m_WorkerThread.join();
#endif

		// Destroy musicTr pools:
		for (auto* track : m_SFXPool)
			MIX_DestroyTrack(track);
		for (auto* track : m_MusicTrackPool)
			MIX_DestroyTrack(track);

		// Destroy audio container:
		for (auto& [id, audio] : m_LoadedAudios)
			MIX_DestroyAudio(audio);

		// Destroy Mixer:
		if (m_Mixer)
			MIX_DestroyMixer(m_Mixer);

		MIX_Quit();
	}

	void SDLSoundSysImpl::Play(ge::Sound_Id soundId, float volume, ge::SoundCategory category)
	{
#ifdef __EMSCRIPTEN__
		ExecutePlay(soundId, std::clamp(volume, 0.f, 1.f), category);
#else
		{
			std::lock_guard<std::mutex> lock(m_AudioStateMutex);
			const float clampedVolume{ std::clamp(volume, 0.f, 1.f) };
			m_PlayQueue.push(PlayRequest{ soundId, clampedVolume, category });
		} // lock goes out of scope

		// and the Worker/Sound thread is notified VIA the condition variable
		m_Cv.notify_one();
#endif
	}

	void SDLSoundSysImpl::RegisterSound(ge::Sound_Id id, const std::string& fileName)
	{
		// "Guard" against data races happening due to m_FileNames
		//assert(m_PlayQueue.empty());

#ifndef __EMSCRIPTEN__
		std::lock_guard<std::mutex> lock(m_AudioStateMutex);
#endif // !__EMSCRIPTEN__


		// If a sound was already registered under this id, destroy it FIRST
		if (auto it{ m_LoadedAudios.find(id) }; it != m_LoadedAudios.end())
		{
			MIX_DestroyAudio(it->second);
			m_LoadedAudios.erase(it);
		}

		// Create/Replace
		m_FileNames[id] = fileName;
		LoadAndCache(id, fileName);
	}

	void SDLSoundSysImpl::StopAll()
	{
		std::lock_guard<std::mutex> lock(m_AudioStateMutex);

		// Stop SFX
		for (auto* sfxTr : m_SFXPool)
			MIX_StopTrack(sfxTr, 0);
		// Stop Music
		for (auto* musicTr : m_MusicTrackPool) 
			MIX_StopTrack(musicTr, 0);

		std::queue<PlayRequest> empty;
		std::swap(m_PlayQueue, empty); // Empty the play queue!
	}

	void SDLSoundSysImpl::SetMuted(bool muted)
	{
		m_Muted.store(muted);

		std::lock_guard<std::mutex> lock(m_AudioStateMutex);
		for (auto* sfxTr : m_SFXPool)
		{
			MIX_SetTrackGain(sfxTr, muted ? 0.f : 1.f);
		}
		for (auto* musicTr : m_MusicTrackPool)
		{
			MIX_SetTrackGain(musicTr, muted ? 0.f : 1.f);
		}
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

	void SDLSoundSysImpl::ExecutePlay(const ge::Sound_Id soundId, const float volume, ge::SoundCategory category)
	{
		if (m_Muted.load()) // Skip playing at all when muted
			return;

		MIX_Audio* audio{ nullptr };

		{
#ifndef __EMSCRIPTEN__
			std::lock_guard<std::mutex> lock(m_AudioStateMutex);
#endif // !1

			// Already loaded:
			const auto it{ m_LoadedAudios.find(soundId) };
			audio = (it != m_LoadedAudios.end()) ? it->second : nullptr;

			// If doesn't exist -> LOAD by cached filename
			if (!audio)
			{
				const auto fileIt{ m_FileNames.find(soundId) };

				// If filename has not been registered, early out...
				if (fileIt == m_FileNames.end())
				{
					return;	
				}

				// If filename exists, Load the sound for next time
				audio = LoadAndCache(soundId, fileIt->second);
				if (!audio)
					return;
			}
		} // lock released

		MIX_Track* track{ nullptr };
		int loops{ 0 };

		// -------------------------------------------------------------
		// Branches depending on category - Music always loops, SFX doesn't
		// -------------------------------------------------------------
		if (category == ge::SoundCategory::Music)
		{
			// Get musicTr from musicTr pool
			track = m_MusicTrackPool[0]; // Only one musicTr can play at a time so...
			loops = -1; // loop infinitely
		}
		else
		{
			// Get musicTr from musicTr pool
			track = m_SFXPool[m_NextSFXTrack];
			// and update next musicTr
			m_NextSFXTrack = (m_NextSFXTrack + 1) % m_SFXPool.size();
			loops = 0;
		}

		// Play
		MIX_SetTrackAudio(track, audio);
		MIX_SetTrackGain(track, volume); // volume is already clamped from the queue.push() call in Play()
		MIX_PlayTrack(track, loops); // 0 for SFX, -1 for Music
	}

#ifndef __EMSCRIPTEN__
	void SDLSoundSysImpl::WorkerLoop()
	{
		while (true)
		{
			PlayRequest request; // don't intialize - this will happen each frame on the worker thread, also - no need
			{
				std::unique_lock<std::mutex> lock(m_AudioStateMutex);

				// WAIT UNTIL WHOLE QUEUE IS PROCESSED (ExecutePlay() is called for EVERY PlayRequest)
				m_Cv.wait(lock, [this]() 
					{
						return !m_PlayQueue.empty() || m_ShouldQuit.load(); 
					});

				if (m_ShouldQuit.load() && m_PlayQueue.empty())
					return;

				request = m_PlayQueue.front();
				m_PlayQueue.pop();
			} // lock/mutex goes out of scope

			ExecutePlay(request.soundId, request.volume, request.category);
		}
	}
#endif
}

bombGame::SDLSoundSystem::SDLSoundSystem()
	:m_Impl{ std::make_unique<SDLSoundSysImpl>() }
{
}

bombGame::SDLSoundSystem::~SDLSoundSystem()
{
	m_Impl.reset();
}

void bombGame::SDLSoundSystem::Play(const ge::Sound_Id soundId, const float volume, ge::SoundCategory category)
{
	m_Impl->Play(soundId, volume, category);
}

void bombGame::SDLSoundSystem::RegisterSound(ge::Sound_Id id, const std::string& fileName)
{
	m_Impl->RegisterSound(id, fileName);
}

void bombGame::SDLSoundSystem::StopAll()
{
	m_Impl->StopAll();
}

void bombGame::SDLSoundSystem::SetMuted(bool muted)
{
	m_Impl->SetMuted(muted);
}

bool bombGame::SDLSoundSystem::IsMuted() const
{
	return m_Impl->IsMuted();
}
