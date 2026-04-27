#include "SDLSoundSystem.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

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

	};
	SDLSoundSysImpl::SDLSoundSysImpl()
	{
	}
	SDLSoundSysImpl::~SDLSoundSysImpl()
	{
	}
	void SDLSoundSysImpl::Play(const ge::Sound_Id soundId, const float volume)
	{
		soundId;
		volume;
	}
	void SDLSoundSysImpl::RegisterSound(ge::Sound_Id id, const std::string& fileName)
	{
		id;
		fileName;
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
