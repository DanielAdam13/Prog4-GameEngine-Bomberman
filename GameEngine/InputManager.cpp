#include <SDL3/SDL.h>
#include "InputManager.h"
#include <backends/imgui_impl_sdl3.h>
#include <cmath>

using namespace ge;

bool InputManager::ProcessInput(float deltaTime)
{
	UpdateController(m_ControllerIndex);

	const bool* keyState{ SDL_GetKeyboardState(nullptr) };

	// --- Pressed Keyboard Bindings ---
#pragma region PressedKeyBindingCommands
	for (auto& keyBinding : m_KeyboardBindings)
	{
		bool shouldExecute{ false };

		switch (keyBinding.triggerType)
		{
		case InputTrigger::Pressed:
			shouldExecute = keyState[keyBinding.key];
			break;
		case InputTrigger::Down:
			// Down in event loop
			break;
		case InputTrigger::Up:
			// Down in Event Loop
			break;
		}
		if (shouldExecute)
			keyBinding.command->Execute(deltaTime);
	}
#pragma endregion

	// --- Controller bindings ---
#pragma region ControllerCommands
	for (auto& controllerBinding : m_ControllerBindings)
	{
		bool shouldExecute{ false };

		switch (controllerBinding.triggerType)
		{
		case InputTrigger::Down:    
			shouldExecute = IsButtonDownThisFrame(controllerBinding.button); 
			break;
		case InputTrigger::Up:     
			shouldExecute = IsButtonUpThisFrame(controllerBinding.button);  
			break;
		case InputTrigger::Pressed: 
			shouldExecute = IsButtonPressed(controllerBinding.button);    
			break;
		}

		if (shouldExecute)
			controllerBinding.command->Execute(deltaTime);
	}

	if (m_ControllerConnected && m_LeftStickCommand)
		m_LeftStickCommand->Execute(deltaTime);
#pragma endregion

	SDL_Event e;
	while (SDL_PollEvent(&e)) 
	{
		if (e.type == SDL_EVENT_QUIT)
			return false;

		// --- Up and Down Keyboard Bindings ---
#pragma region DownUpKeyBindingCommands
		if (e.type == SDL_EVENT_KEY_DOWN) 
		{
			// 1. If triggerType type matches
			// and
			// 2. If SDL key matches the keybinding's key
			for (auto& keyBinding : m_KeyboardBindings)
			{
				if (keyBinding.triggerType == InputTrigger::Down &&
					e.key.scancode == keyBinding.key)
				{
					keyBinding.command->Execute(deltaTime);
				}
			}
		}

		if (e.type == SDL_EVENT_KEY_UP)
		{
			for (auto& keyBinding : m_KeyboardBindings)
			{
				// 1. If triggerType type matches
				// and
				// 2. If SDL key matches the keybinding's key
				if (keyBinding.triggerType == InputTrigger::Up &&
					e.key.scancode == keyBinding.key)
				{
					keyBinding.command->Execute(deltaTime);
				}
			}
		}

		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) 
		{
			
		}
		// etc...
#pragma endregion

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

void InputManager::BindKeyboardCommand(SDL_Scancode key, InputTrigger trigger, std::unique_ptr<GameObjectCommand> command)
{
	m_KeyboardBindings.push_back(KeyBoardBinding{ key, trigger, std::move(command) });
}

void InputManager::BindControllerCommand(unsigned int button, InputTrigger trigger, std::unique_ptr<GameObjectCommand> command)
{
	m_ControllerBindings.push_back(ControllerBinding{ button, trigger, std::move(command) });
}

void InputManager::BindControllerStickCommand(std::unique_ptr<GameObjectCommand> command)
{
	m_LeftStickCommand = std::move(command);
}

void InputManager::UnbindAll()
{
	m_KeyboardBindings.clear();
	m_ControllerBindings.clear();
	m_LeftStickCommand.reset();
}

void InputManager::UnbindAllKeyboard()
{
	m_KeyboardBindings.clear();
}

void InputManager::UnbindAllController()
{
	m_ControllerBindings.clear();
	m_LeftStickCommand.reset();
}

void InputManager::UnbindAllCommandsOfTarget(GameObject* target)
{
	// Unbind Keyboard
	std::erase_if(m_KeyboardBindings, [target](const KeyBoardBinding& binding)
		{
			const auto* command{ binding.command.get() };
			return command && command->GetCommandTarget() == target; // If matching target and not null
		});

	// Unbind Controller
	std::erase_if(m_ControllerBindings, [target](const ControllerBinding& binding)
		{
			const auto* command{ binding.command.get() };
			return command && command->GetCommandTarget() == target;
		});

	// Unbind Controller Stick
	if (m_LeftStickCommand)
	{
		const auto* command{ m_LeftStickCommand.get() };
		if (command && command->GetCommandTarget() == target)
			m_LeftStickCommand.reset();
	}
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
