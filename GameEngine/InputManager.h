#pragma once

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <Windows.h>
	#include <Xinput.h>
#endif

#include "Singleton.h"

namespace ge
{
	class InputManager final : public Singleton<InputManager>
	{
	public:
		bool ProcessInput();
		void UpdateController(unsigned int controllerIndex = 0);

		// Keyboard
		bool IsKeyDown(SDL_Scancode) const;

		// Controller
		bool IsButtonDownThisFrame(unsigned int button) const noexcept;
		bool IsButtonUpThisFrame(unsigned int button) const noexcept;
		bool IsButtonPressed(unsigned int button) const noexcept;

		bool IsControllerConnected() const noexcept { return m_ControllerConnected; }

	private:
#ifdef _WIN32
		XINPUT_STATE m_PreviousState{};
		XINPUT_STATE m_CurrentState{};
		WORD m_ButtonsPressedThisFrame{ 0 };
		WORD m_ButtonsReleasedThisFrame{ 0 };
		bool m_ControllerConnected{ false };
#endif

		unsigned int m_ControllerIndex{ 0 };
	};

}
