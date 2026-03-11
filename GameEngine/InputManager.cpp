#include <SDL3/SDL.h>
#include "InputManager.h"
#include <backends/imgui_impl_sdl3.h>
#include <cmath>

using namespace ge;

bool InputManager::ProcessInput()
{
	UpdateController(m_ControllerIndex);

	SDL_Event e;
	while (SDL_PollEvent(&e)) 
	{
		if (e.type == SDL_EVENT_QUIT)
			return false;

		if (e.type == SDL_EVENT_KEY_DOWN) 
		{
			
		}
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) 
		{
			
		}
		// etc...

		ImGui_ImplSDL3_ProcessEvent(&e);
	}

	return true;
}

void InputManager::UpdateController(unsigned int controllerIndex)
{
	m_ControllerIndex = controllerIndex;

	CopyMemory(&m_PreviousState, &m_CurrentState, sizeof(XINPUT_STATE));
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));

	// For runtime controller connection + result check
	const DWORD result{ XInputGetState(m_ControllerIndex, &m_CurrentState) };
	m_ControllerConnected = (result == ERROR_SUCCESS);

	if (!m_ControllerConnected)
	{
		// No controller - clear everything for next call
		ZeroMemory(&m_PreviousState, sizeof(XINPUT_STATE));
		m_ButtonsPressedThisFrame = 0;
		m_ButtonsReleasedThisFrame = 0;
		return;
	}

	// Difference between current and previous
	auto buttonChanges{ m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons };
	m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
	m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
}

bool InputManager::IsKeyDown(SDL_Scancode) const
{
	return false;
}

bool InputManager::IsButtonDownThisFrame(unsigned int button) const noexcept
{
	return m_ButtonsPressedThisFrame & button;
}

bool InputManager::IsButtonUpThisFrame(unsigned int button) const noexcept
{
	return m_ButtonsReleasedThisFrame & button;
}

bool InputManager::IsButtonPressed(unsigned int button) const noexcept
{
	return m_CurrentState.Gamepad.wButtons & button;
}

glm::vec2 InputManager::GetLeftStick()
{
	// should clamp between -1.f : 1.f for safety is using in animation or physics
	float x{ m_CurrentState.Gamepad.sThumbLX / STICK_MAX_VALUE };
	float y{ m_CurrentState.Gamepad.sThumbLY / STICK_MAX_VALUE };
	ApplyRadialDeadzone(x, y, STICK_DEADZONE);
	return { x, y };
}

glm::vec2 InputManager::GetRightStick()
{
	// should clamp between -1.f : 1.f for safety is using in animation or physics
	float x{ m_CurrentState.Gamepad.sThumbRX / STICK_MAX_VALUE };
	float y{ m_CurrentState.Gamepad.sThumbRY / STICK_MAX_VALUE };
	ApplyRadialDeadzone(x, y, STICK_DEADZONE);
	return { x, y };
}

void InputManager::ApplyRadialDeadzone(float& x, float& y, float deadzone)
{
	const float magnitude{ std::sqrt(x * x + y * y) };
	if (magnitude < deadzone)
	{
		x = 0.f;
		y = 0.f;
		return;
	}

	// Apply deadzone
	const float scale{ (magnitude - deadzone) / (1.f - deadzone) / magnitude };
	x *= scale;
	y *= scale;
}
