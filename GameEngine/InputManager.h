#pragma once

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <Windows.h>
	#include <Xinput.h>
#endif

#include "Singleton.h"
#include <glm/glm.hpp>

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

		glm::vec2 GetLeftStick();
		glm::vec2 GetRightStick();

		bool IsControllerConnected() const noexcept { return m_ControllerConnected; }

	private:
#ifdef _WIN32
		XINPUT_STATE m_PreviousState{};
		XINPUT_STATE m_CurrentState{};
		WORD m_ButtonsPressedThisFrame{ 0 };
		WORD m_ButtonsReleasedThisFrame{ 0 };
#endif

		bool m_ControllerConnected{ false };
		unsigned int m_ControllerIndex{ 0 };

		static constexpr float STICK_DEADZONE{ 0.1f }; // 10%
		void ApplyRadialDeadzone(float& x, float& y, float deadzone);
		static constexpr float STICK_MAX_VALUE{ 32767.f };
	};

}
