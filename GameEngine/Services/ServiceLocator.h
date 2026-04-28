#pragma once
#include "Singleton.h"
#include "Services/SoundSystem.h"
#include "Services/InputManager.h"

#include <memory>

namespace ge
{
	class SoundSystem;

	// A FULLY STATIC singleton class that manages services such as SoundSystem or InputManager
	class ServiceLocator final
	{
	public:
		ServiceLocator() = delete;
		~ServiceLocator() = delete;
		ServiceLocator(const ServiceLocator&) = delete;
		ServiceLocator(ServiceLocator&&) = delete;
		ServiceLocator& operator=(const ServiceLocator& other) = delete;
		ServiceLocator& operator=(ServiceLocator&& other) = delete;

		// Needs to be EXPLICITELY called before SDL_Quit() in GameEngine
		static void Shutdown();

		static void RegisterInputManager(std::unique_ptr<InputManager>&& inMan);
		static InputManager& GetInputManager();

		static void RegisterSoundSystem(std::unique_ptr<SoundSystem>&& soundSys);
		static SoundSystem& GetSoundSystem();

	private:
		static std::unique_ptr<SoundSystem> SoundSystem_Instance;
		static std::unique_ptr<InputManager> InputManager_Instance;
	};
}