#pragma once
#include "Singleton.h"

// SDL is multiplatform
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <memory>

namespace ge
{
	class InputManagerImpl;
	class GameObjectCommand;
	class GameObject;

	// PIMPL CLASS
	class InputManager final
	{
	public:
		InputManager();
		~InputManager();

		bool ProcessInput(float deltaTime);
		void UpdateController(unsigned int controllerIndex = 0);

		// Keyboard
		bool IsKeyDown(SDL_Scancode) const;

		// Controller
		bool IsButtonDownThisFrame(unsigned int button) const noexcept;
		bool IsButtonUpThisFrame(unsigned int button) const noexcept;
		bool IsButtonPressed(unsigned int button) const noexcept;

		glm::vec2 GetLeftStick() const;
		glm::vec2 GetRightStick() const;

		bool IsControllerConnected() const noexcept;

		// Input Trigger should live here
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
		// Acutal PIMPL implementation in .cpp
		std::unique_ptr<InputManagerImpl> m_Impl;

	};

	// Controller bit masks with custom Button names
	namespace ControllerButton
	{
		constexpr unsigned int DpadUp = 0x0001;
		constexpr unsigned int DpadDown = 0x0002;
		constexpr unsigned int DpadLeft = 0x0004;
		constexpr unsigned int DpadRight = 0x0008;
		constexpr unsigned int Start = 0x0010;
		constexpr unsigned int Back = 0x0020;
		constexpr unsigned int LeftThumb = 0x0040;
		constexpr unsigned int RightThumb = 0x0080;
		constexpr unsigned int LeftShoulder = 0x0100;
		constexpr unsigned int RightShoulder = 0x0200;
		constexpr unsigned int A = 0x1000;
		constexpr unsigned int B = 0x2000;
		constexpr unsigned int X = 0x4000;
		constexpr unsigned int Y = 0x8000;
	}
}
