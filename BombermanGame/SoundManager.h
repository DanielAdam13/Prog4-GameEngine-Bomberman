#pragma once
#include <Observer.h>

#include <Services/SoundSystem.h>
#include "GameEvents.h"

#include <unordered_map>

namespace ge
{
	class SoundSystem;
	class GameObject;
}

namespace bombGame
{
	// A fully static singleton OBSERVER class playing sounds via an EventQueue 
	class SoundManager final : public ge::IObserver
	{
	public:
		SoundManager();
		virtual ~SoundManager() override = default;
		SoundManager(const SoundManager&) = delete;
		SoundManager(SoundManager&&) = delete;
		SoundManager& operator=(const SoundManager& other) = delete;
		SoundManager& operator=(SoundManager&& other) = delete;

		virtual void Notify(int eventId, ge::GameObject*) override;

		struct SoundMapping
		{
			ge::Sound_Id soundId;
			float volume;
		};

		// Simply maps an event id to a sound id, taking volume
		void RegisterMapping(EventId eventId, ge::Sound_Id soundId, float volume);

	private:
		std::unordered_map<EventId, SoundManager::SoundMapping> m_SoundMappings{};

	};
}