#include "InputManager.h"
#include "Commands/Command.h"

#include <backends/imgui_impl_sdl3.h>
#include <cmath>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Xinput.h>
#endif
#include <array>

	namespace ge
	{
		// DEFINE THE IMPLEMENTATION CLASS FIRST
		class InputManagerImpl final
		{
		public:
			InputManagerImpl() = default;
			// Emscripten build needs SDL gamepad cleanup
			~InputManagerImpl();

			bool ProcessInput(float deltaTime);
			void UpdateAllControllers();
			void UpdateController(unsigned int controllerIndex);

			// Keyboard
			bool IsKeyDown(SDL_Scancode) const;

			// Controller
			bool IsButtonDownThisFrame(unsigned int controllerIndex, unsigned int button) const noexcept;
			bool IsButtonUpThisFrame(unsigned int controllerIndex, unsigned int button) const noexcept;
			bool IsButtonPressed(unsigned int controllerIndex, unsigned int button) const noexcept;

			void BindKeyboardCommand(SDL_Scancode key, InputManager::InputTrigger trigger, std::unique_ptr<Command> command);
			void BindControllerCommand(unsigned int controllerIndex,
				unsigned int button, InputManager::InputTrigger trigger, std::unique_ptr<Command> command);

			int GetControllerCount() const noexcept;
			bool IsControllerConnected(unsigned int controllerIndex) const noexcept;

			void UnbindAll();
			void UnbindAllKeyboard();
			void UnbindAllController();
			void UnbindAllCommandsOfTarget(GameObject* target);

		private:
			struct ControllerState
			{
#ifdef _WIN32
				XINPUT_STATE previousState{};
				XINPUT_STATE currentState{};

#else
				SDL_Gamepad* pGamepad{ nullptr };
				uint32_t sdlButtonsCurrent{ 0 };
				uint32_t sdlButtonsPrevious{ 0 };
#endif

				uint32_t buttonsPressedThisFrame{ 0 };
				uint32_t buttonsReleasedThisFrame{ 0 };

				bool controllerConnected{ false };
			};
	
			static constexpr unsigned int MaxControllers{ 2 };
			std::array<ControllerState, MaxControllers> m_Controllers{};

			struct KeyBoardBinding
			{
				SDL_Scancode key;
				InputManager::InputTrigger triggerType;
				std::unique_ptr<Command> command;
			};

			struct ControllerBinding
			{
				unsigned int controllerIndex;
				unsigned int button;
				InputManager::InputTrigger triggerType;
				std::unique_ptr<Command> command;
			};

			std::vector<KeyBoardBinding> m_KeyboardBindings{};
			std::vector<ControllerBinding> m_ControllerBindings{};

	#ifndef _WIN32
			void MapButton(unsigned int controllerIndex, SDL_GamepadButton sdlButton, unsigned int bit);
	#endif // !WIN32
		};


		// Then implement Input Manager methods
		InputManager::InputManager()
			:m_Impl{ std::make_unique<InputManagerImpl>() }
		{}
		InputManager::~InputManager() = default;

		bool InputManager::ProcessInput(float deltaTime)
		{
			return m_Impl->ProcessInput(deltaTime);;
		}
		void InputManager::UpdateController(unsigned int controllerIndex)
		{
			m_Impl->UpdateController(controllerIndex);
		}
		bool InputManager::IsKeyDown(SDL_Scancode) const
		{
			return m_Impl->IsKeyDown({});
		}
		bool InputManager::IsButtonDownThisFrame(unsigned int controllerIndex, unsigned int button) const noexcept
		{
			return m_Impl->IsButtonDownThisFrame(controllerIndex, button);
		}
		bool InputManager::IsButtonUpThisFrame(unsigned int controllerIndex, unsigned int button) const noexcept
		{
			return m_Impl->IsButtonUpThisFrame(controllerIndex, button);
		}
		bool InputManager::IsButtonPressed(unsigned int controllerIndex, unsigned int button) const noexcept
		{
			return m_Impl->IsButtonPressed(controllerIndex, button);
		}
		bool InputManager::IsControllerConnected(unsigned int controllerIndex) const noexcept
		{
			return m_Impl->IsControllerConnected(controllerIndex);
		}
		int InputManager::GetControllerCount() const noexcept
		{
			return m_Impl->GetControllerCount();
		}
		void InputManager::BindKeyboardCommand(SDL_Scancode key, InputTrigger trigger, std::unique_ptr<Command> command)
		{
			m_Impl->BindKeyboardCommand(key, trigger, std::move(command));
		}
		void InputManager::BindControllerCommand(unsigned int controllerIndex,
			unsigned int button, InputTrigger trigger, std::unique_ptr<Command> command)
		{
			m_Impl->BindControllerCommand(controllerIndex, button, trigger, std::move(command));
		}
		void InputManager::UnbindAll()
		{
			m_Impl->UnbindAll();
		}
		void InputManager::UnbindAllKeyboard()
		{
			m_Impl->UnbindAllKeyboard();
		}
		void InputManager::UnbindAllController()
		{
			m_Impl->UnbindAllController();
		}
		void InputManager::UnbindAllCommandsOfTarget(GameObject* target)
		{
			m_Impl->UnbindAllCommandsOfTarget(target);
		}
	}


	// --------------------------------
	// ACTUAL Implementation of public methods
	// --------------------------------
	namespace ge
	{
		InputManagerImpl::~InputManagerImpl()
		{
	#ifndef _WIN32
			for (auto& c : m_Controllers)
			{
				if (c.pGamepad)
				{
					SDL_CloseGamepad(c.pGamepad);
					c.pGamepad = nullptr;
				}
			}
	#endif // !WIN32

		}

		bool InputManagerImpl::ProcessInput(float deltaTime)
		{
			UpdateAllControllers();

			const bool* keyState{ SDL_GetKeyboardState(nullptr) };

			// --- Pressed Keyboard Bindings ---
	#pragma region PressedKeyBindingCommands
			for (auto& keyBinding : m_KeyboardBindings)
			{
				bool shouldExecute{ false };

				switch (keyBinding.triggerType)
				{
				case InputManager::InputTrigger::Pressed:
					shouldExecute = keyState[keyBinding.key];
					break;
				case InputManager::InputTrigger::Down:
					// Down in event loop
					break;
				case InputManager::InputTrigger::Up:
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
				case InputManager::InputTrigger::Down:
					shouldExecute = IsButtonDownThisFrame(controllerBinding.controllerIndex,
						controllerBinding.button);
					break;
				case InputManager::InputTrigger::Up:
					shouldExecute = IsButtonUpThisFrame(controllerBinding.controllerIndex,
						controllerBinding.button);
					break;
				case InputManager::InputTrigger::Pressed:
					shouldExecute = IsButtonPressed(controllerBinding.controllerIndex,
						controllerBinding.button);
					break;
				}

				if (shouldExecute)
					controllerBinding.command->Execute(deltaTime);
			}
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
						if (keyBinding.triggerType == InputManager::InputTrigger::Down &&
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
						if (keyBinding.triggerType == InputManager::InputTrigger::Up &&
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

		void InputManagerImpl::UpdateAllControllers()
		{
			for (unsigned int i{}; i < MaxControllers; ++i)
			{
				UpdateController(i);
			}
		}

		void InputManagerImpl::UpdateController(unsigned int controllerIndex)
		{
			auto& state{ m_Controllers[controllerIndex] };
	#ifdef _WIN32
			CopyMemory(&state.previousState, &state.currentState,
				sizeof(XINPUT_STATE));
			ZeroMemory(&state.currentState, sizeof(XINPUT_STATE));

			// For runtime controller connection + result check
			const DWORD result{ XInputGetState(controllerIndex, &state.currentState) };
			state.controllerConnected = (result == ERROR_SUCCESS);

			if (!state.controllerConnected)
			{
				// No controller - clear everything for next call
				ZeroMemory(&state.previousState, sizeof(XINPUT_STATE));
				state.buttonsPressedThisFrame = 0;
				state.buttonsReleasedThisFrame = 0;
				return;
			}

			// Difference between current and previous
			auto buttonChanges{ state.currentState.Gamepad.wButtons ^
				state.previousState.Gamepad.wButtons };
			state.buttonsPressedThisFrame =
				buttonChanges & state.currentState.Gamepad.wButtons;
			state.buttonsReleasedThisFrame =
				buttonChanges & (~state.currentState.Gamepad.wButtons);
	#else
			// Open Gamepad if not yet connected (for runtime)
			if (!state.controllerConnected)
			{
				int count{};
				SDL_JoystickID* gamepads{ SDL_GetGamepads(&count) };

				if (gamepads && count > static_cast<int>(controllerIndex))
					state.pGamepad = SDL_OpenGamepad(gamepads[controllerIndex]);

				SDL_free(gamepads);
			}

			state.controllerConnected = (state.pGamepad != nullptr);
			if (!state.controllerConnected)
			{
				state.sdlButtonsCurrent = 0;
				state.sdlButtonsPrevious = 0;
				state.buttonsPressedThisFrame = 0;
				state.buttonsReleasedThisFrame = 0;
				return;
			}

			state.sdlButtonsPrevious = state.sdlButtonsCurrent;
			state.sdlButtonsCurrent = 0;

			// Map all buttons to the defined gamepad in InputManager bits
			MapButton(controllerIndex, SDL_GAMEPAD_BUTTON_DPAD_UP, ControllerButton::DpadUp);
			MapButton(controllerIndex, SDL_GAMEPAD_BUTTON_DPAD_DOWN, ControllerButton::DpadDown);
			MapButton(controllerIndex, SDL_GAMEPAD_BUTTON_DPAD_LEFT, ControllerButton::DpadLeft);
			MapButton(controllerIndex, SDL_GAMEPAD_BUTTON_DPAD_RIGHT, ControllerButton::DpadRight);
			MapButton(controllerIndex, SDL_GAMEPAD_BUTTON_START, ControllerButton::Start);
			MapButton(controllerIndex, SDL_GAMEPAD_BUTTON_BACK, ControllerButton::Back);
			MapButton(controllerIndex, SDL_GAMEPAD_BUTTON_LEFT_STICK, ControllerButton::LeftThumb);
			MapButton(controllerIndex, SDL_GAMEPAD_BUTTON_RIGHT_STICK, ControllerButton::RightThumb);
			MapButton(controllerIndex, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, ControllerButton::LeftShoulder);
			MapButton(controllerIndex, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, ControllerButton::RightShoulder);
			MapButton(controllerIndex, SDL_GAMEPAD_BUTTON_SOUTH, ControllerButton::A);
			MapButton(controllerIndex, SDL_GAMEPAD_BUTTON_EAST, ControllerButton::B);
			MapButton(controllerIndex, SDL_GAMEPAD_BUTTON_WEST, ControllerButton::X);
			MapButton(controllerIndex, SDL_GAMEPAD_BUTTON_NORTH, ControllerButton::Y);

			uint32_t buttonChanges{ state.sdlButtonsCurrent ^ state.sdlButtonsPrevious };
			buttonsPressedThisFrame = buttonChanges & state.sdlButtonsCurrent;
			buttonsReleasedThisFrame = buttonChanges & (~state.sdlButtonsCurrent);
	#endif
		}

		bool InputManagerImpl::IsKeyDown(SDL_Scancode) const
		{
			return false;
		}

		bool InputManagerImpl::IsButtonDownThisFrame(unsigned int controllerIndex, unsigned int button) const noexcept
		{
			return m_Controllers[controllerIndex].buttonsPressedThisFrame & button;
		}

		bool InputManagerImpl::IsButtonUpThisFrame(unsigned int controllerIndex, unsigned int button) const noexcept
		{
			return m_Controllers[controllerIndex].buttonsReleasedThisFrame & button;
		}

		int InputManagerImpl::GetControllerCount() const noexcept
		{
			int count{ 0 };
			for (unsigned int i{}; i < MaxControllers; ++i)
			{
				if (IsControllerConnected(i))
					++count;
			}
			return count;
		}

		bool InputManagerImpl::IsControllerConnected(unsigned int controllerIndex) const noexcept
		{
			return m_Controllers[controllerIndex].controllerConnected;
		}

		bool InputManagerImpl::IsButtonPressed(unsigned int controllerIndex, unsigned int button) const noexcept
		{
	#ifdef _WIN32
			return m_Controllers[controllerIndex].currentState.Gamepad.wButtons & button;
	#else
			return m_SDLButtonsCurrent & button;
	#endif
		}

		void InputManagerImpl::BindKeyboardCommand(SDL_Scancode key, InputManager::InputTrigger trigger, std::unique_ptr<Command> command)
		{
			m_KeyboardBindings.push_back(KeyBoardBinding{ key, trigger, std::move(command) });
		}

		void InputManagerImpl::BindControllerCommand(unsigned int controllerIndex,
			unsigned int button, InputManager::InputTrigger trigger, std::unique_ptr<Command> command)
		{
			m_ControllerBindings.push_back(ControllerBinding{ controllerIndex, button, trigger, std::move(command) });
		}

		void InputManagerImpl::UnbindAll()
		{
			m_KeyboardBindings.clear();
			m_ControllerBindings.clear();
		}

		void InputManagerImpl::UnbindAllKeyboard()
		{
			m_KeyboardBindings.clear();
		}

		void InputManagerImpl::UnbindAllController()
		{
			m_ControllerBindings.clear();
		}

		void InputManagerImpl::UnbindAllCommandsOfTarget(GameObject* target)
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
		}

	#ifndef _WIN32
		void InputManagerImpl::MapButton(unsigned int controllerIndex, SDL_GamepadButton sdlButton, unsigned int bit)
		{
			auto& state{ m_Controllers[controllerIndex] };
			if (SDL_GetGamepadButton(state.pGamepad, sdlButton))
				state.sdlButtonsCurrent |= bit;
		}
	#endif // !WIN32
	}