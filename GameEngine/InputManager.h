#pragma once

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <Windows.h>
	#include <Xinput.h>
#endif

#include <SDL3/SDL.h>

#include "Singleton.h"
#include <glm/glm.hpp>
#include <memory>

#include "Commands/GameObjectCommand.h"

namespace ge
{

	class InputManager final : public Singleton<InputManager>
	{
	public:
		bool ProcessInput(float deltaTime);
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

		enum class InputTrigger
		{
			Down,
			Up,
			Pressed
		};

		void BindKeyboardCommand(SDL_Scancode key, InputTrigger trigger, std::unique_ptr<GameObjectCommand> command);
		void BindControllerCommand(unsigned int button, InputTrigger trigger, std::unique_ptr<GameObjectCommand> command);
		void BindControllerStickCommand(std::unique_ptr<GameObjectCommand> command);

		void UnbindAll();
		void UnbindAllKeyboard();
		void UnbindAllController();
		void UnbindAllCommandsOfTarget(GameObject* target);

	private:
#ifdef _WIN32
		XINPUT_STATE m_PreviousState{};
		XINPUT_STATE m_CurrentState{};
		WORD m_ButtonsPressedThisFrame{ 0 };
		WORD m_ButtonsReleasedThisFrame{ 0 };
#else
		SDL_Gamepad* m_pGamepad{ nullptr };
		uint32_t m_SDLButtonsCurrent{ 0 };
		uint32_t m_SDLButtonsPrevious{ 0 };
#endif

		bool m_ControllerConnected{ false };
		unsigned int m_ControllerIndex{ 0 };

		static constexpr float STICK_DEADZONE{ 0.1f }; // 10%
		void ApplyRadialDeadzone(float& x, float& y, float deadzone);
		static constexpr float STICK_MAX_VALUE{ 32767.f };

		struct KeyBoardBinding
		{
			SDL_Scancode key;
			InputTrigger triggerType;
			std::unique_ptr<GameObjectCommand> command;
		};

		struct ControllerBinding
		{
			unsigned int button;
			InputTrigger triggerType;
			std::unique_ptr<GameObjectCommand> command;
		};

		std::vector<KeyBoardBinding> m_KeyboardBindings{};
		std::vector<ControllerBinding> m_ControllerBindings{};

		std::unique_ptr<GameObjectCommand> m_LeftStickCommand{};
	};

}
