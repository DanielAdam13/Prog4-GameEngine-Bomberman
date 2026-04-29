#include "SDLSoundSystem.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <stdexcept>
#include <unordered_map>
#include <algorithm>
#include <vector>

#include <condition_variable>
#include <queue>
#include <thread>
#include <atomic>
#include <cassert>

namespace bombGame
{
	// Type passed to the queue executed from the worker thread
	struct PlayRequest
	{
		ge::Sound_Id soundId;
		float volume;
	};


	class SDLSoundSysImpl final
	{
	public:
		SDLSoundSysImpl();
		~SDLSoundSysImpl();

		// Push the PlayRequest to the play queue
		void Play(const ge::Sound_Id soundId, const float volume);

		// DANGER - Might cause a data race due to writing to m_FileNames while
		// the worker thread might be processing it
		void RegisterSound(ge::Sound_Id id, const std::string& fileName);

	private:
		// Loads the file for ID. Returns the loaded audio. If already cached -> return the cache
		MIX_Audio* LoadAndCache(ge::Sound_Id soundId, const std::string& fileName);

		MIX_Mixer* m_Mixer{ nullptr };

		std::unordered_map<ge::Sound_Id, MIX_Audio*> m_LoadedAudios{};
		std::unordered_map<ge::Sound_Id, std::string> m_FileNames{};

		static constexpr size_t TrackPoolSize{ 16 };
		std::vector<MIX_Track*> m_TrackPool{};
		size_t m_NextTrack{ 0 };

		// Threading:
		std::queue<PlayRequest> m_PlayQueue{};
		std::mutex m_Mutex{}; // Mutex needed for locks
		std::condition_variable m_Cv{}; // Condition variable to wait on for ExecutePlay()
		std::atomic<bool> m_ShouldQuit{ false }; // for when window is closed
		std::thread m_WorkerThread{}; // Thread for Load and Play()

		// Processes Queue by the workrer thread
		void WorkerLoop(); 

		// Actual sequential PlayTrack logic
		void ExecutePlay(const ge::Sound_Id soundId, const float volume);
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

		// Initialize worker thread with its dedicated method
		m_WorkerThread = std::thread(&SDLSoundSysImpl::WorkerLoop, this);
	}

	SDLSoundSysImpl::~SDLSoundSysImpl()
	{
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_ShouldQuit.store(true);
		} // lock goes out of scope
		m_Cv.notify_one();
		if (m_WorkerThread.joinable())
			m_WorkerThread.join();

		// Destroy track pool:
		for (auto* track : m_TrackPool)
			MIX_DestroyTrack(track);

		// Destroy audio container:
		for (auto& [id, audio] : m_LoadedAudios)
			MIX_DestroyAudio(audio);

		// Destroy Mixer:
		if (m_Mixer)
			MIX_DestroyMixer(m_Mixer);

		MIX_Quit();
	}

	void SDLSoundSysImpl::Play(const ge::Sound_Id soundId, const float volume)
	{
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			const float clampedVolume{ std::clamp(volume, 0.f, 1.f) };
			m_PlayQueue.push(PlayRequest{ soundId, clampedVolume });
		} // lock goes out of scope

		// and the Worker/Sound thread is notified VIA the condition variable
		m_Cv.notify_one();
	}

	void SDLSoundSysImpl::RegisterSound(ge::Sound_Id id, const std::string& fileName)
	{
		// "Guard" against data races happening due to m_FileNames
		assert(m_PlayQueue.empty());

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

	void SDLSoundSysImpl::ExecutePlay(const ge::Sound_Id soundId, const float volume)
	{
		// Already loaded:
		const auto it{ m_LoadedAudios.find(soundId) };
		MIX_Audio* audio{ (it != m_LoadedAudios.end()) ? it->second : nullptr };

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

		// Get track from track pool
		MIX_Track* track{ m_TrackPool[m_NextTrack] };
		// and update next track
		m_NextTrack = (m_NextTrack + 1) % m_TrackPool.size();

		// Play
		MIX_SetTrackAudio(track, audio);
		MIX_SetTrackGain(track, volume); // volume is already clamped from the queue.push() call in Play()
		MIX_PlayTrack(track, 0); // 0 - default: play once
	}

	void SDLSoundSysImpl::WorkerLoop()
	{
		while (true)
		{
			PlayRequest request; // don't intialize - this will happen each frame on the worker thread, also - no need
			{
				std::unique_lock<std::mutex> lock(m_Mutex);

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

			ExecutePlay(request.soundId, request.volume);
		}
	}
}

bombGame::SDLSoundSystem::SDLSoundSystem()
	:m_Impl{ std::make_unique<SDLSoundSysImpl>() }
{
}

bombGame::SDLSoundSystem::~SDLSoundSystem()
{
	m_Impl.reset();
}

void bombGame::SDLSoundSystem::Play(const ge::Sound_Id soundId, const float volume)
{
	m_Impl->Play(soundId, volume);
}

void bombGame::SDLSoundSystem::RegisterSound(ge::Sound_Id id, const std::string& fileName)
{
	m_Impl->RegisterSound(id, fileName);
}
