	#include "InputManager.h"

	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#define NOMINMAX
		#include <Windows.h>
		#include <Xinput.h>
	#endif

	#include <backends/imgui_impl_sdl3.h>
	#include <cmath>

	#include "Commands/GameObjectCommand.h"

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

			void BindKeyboardCommand(SDL_Scancode key, InputManager::InputTrigger trigger, std::unique_ptr<GameObjectCommand> command);
			void BindControllerCommand(unsigned int button, InputManager::InputTrigger trigger, std::unique_ptr<GameObjectCommand> command);
			void BindControllerStickCommand(std::unique_ptr<GameObjectCommand> command);

			void UnbindAll();
			void UnbindAllKeyboard();
			void UnbindAllController();
			void UnbindAllCommandsOfTarget(GameObject* target);

		private:
	#ifdef _WIN32
			XINPUT_STATE m_PreviousState{};
			XINPUT_STATE m_CurrentState{};
		
	#else
			SDL_Gamepad* m_pGamepad{ nullptr };
			uint32_t m_SDLButtonsCurrent{ 0 };
			uint32_t m_SDLButtonsPrevious{ 0 };
	#endif

			uint32_t m_ButtonsPressedThisFrame{ 0 };
			uint32_t m_ButtonsReleasedThisFrame{ 0 };

			bool m_ControllerConnected{ false };
			unsigned int m_ControllerIndex{ 0 };

			static constexpr float STICK_DEADZONE{ 0.1f }; // 10%
			void ApplyRadialDeadzone(float& x, float& y, float deadzone) const;
			static constexpr float STICK_MAX_VALUE{ 32767.f };

			struct KeyBoardBinding
			{
				SDL_Scancode key;
				InputManager::InputTrigger triggerType;
				std::unique_ptr<GameObjectCommand> command;
			};

			struct ControllerBinding
			{
				unsigned int button;
				InputManager::InputTrigger triggerType;
				std::unique_ptr<GameObjectCommand> command;
			};

			std::vector<KeyBoardBinding> m_KeyboardBindings{};
			std::vector<ControllerBinding> m_ControllerBindings{};

			std::unique_ptr<GameObjectCommand> m_LeftStickCommand{};

	#ifndef _WIN32
			void MapButton(SDL_GamepadButton sdlButton, unsigned int bit);
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
		bool InputManager::IsButtonDownThisFrame(unsigned int button) const noexcept
		{
			return m_Impl->IsButtonDownThisFrame(button);
		}
		bool InputManager::IsButtonUpThisFrame(unsigned int button) const noexcept
		{
			return m_Impl->IsButtonUpThisFrame(button);
		}
		bool InputManager::IsButtonPressed(unsigned int button) const noexcept
		{
			return m_Impl->IsButtonPressed(button);
		}
		glm::vec2 InputManager::GetLeftStick() const
		{
			return m_Impl->GetLeftStick();
		}
		glm::vec2 InputManager::GetRightStick() const
		{
			return m_Impl->GetRightStick();
		}
		bool InputManager::IsControllerConnected() const noexcept
		{
			return m_Impl->IsControllerConnected();
		}
		void InputManager::BindKeyboardCommand(SDL_Scancode key, InputTrigger trigger, std::unique_ptr<GameObjectCommand> command)
		{
			m_Impl->BindKeyboardCommand(key, trigger, std::move(command));
		}
		void InputManager::BindControllerCommand(unsigned int button, InputTrigger trigger, std::unique_ptr<GameObjectCommand> command)
		{
			m_Impl->BindControllerCommand(button, trigger, std::move(command));
		}
		void InputManager::BindControllerStickCommand(std::unique_ptr<GameObjectCommand> command)
		{
			m_Impl->BindControllerStickCommand(std::move(command));
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
			if (m_pGamepad)
			{
				SDL_CloseGamepad(m_pGamepad);
				m_pGamepad = nullptr;
			}
	#endif // !WIN32

		}

		bool InputManagerImpl::ProcessInput(float deltaTime)
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
					shouldExecute = IsButtonDownThisFrame(controllerBinding.button);
					break;
				case InputManager::InputTrigger::Up:
					shouldExecute = IsButtonUpThisFrame(controllerBinding.button);
					break;
				case InputManager::InputTrigger::Pressed:
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

		void InputManagerImpl::UpdateController(unsigned int controllerIndex)
		{
			m_ControllerIndex = controllerIndex;

	#ifdef _WIN32
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
	#else
			// Open Gamepad if not yet connected (for runtime)
			if (!m_pGamepad)
			{
				int count{};
				SDL_JoystickID* gamepads{ SDL_GetGamepads(&count) };

				if (gamepads && count > static_cast<int>(controllerIndex))
					m_pGamepad = SDL_OpenGamepad(gamepads[controllerIndex]);

				SDL_free(gamepads);
			}

			m_ControllerConnected = (m_pGamepad != nullptr);
			if (!m_ControllerConnected)
			{
				m_SDLButtonsCurrent = 0;
				m_SDLButtonsPrevious = 0;
				m_ButtonsPressedThisFrame = 0;
				m_ButtonsReleasedThisFrame = 0;
				return;
			}

			m_SDLButtonsPrevious = m_SDLButtonsCurrent;
			m_SDLButtonsCurrent = 0;

			// Map all buttons to the defined gamepad in InputManager bits
			MapButton(SDL_GAMEPAD_BUTTON_DPAD_UP, ControllerButton::DpadUp);
			MapButton(SDL_GAMEPAD_BUTTON_DPAD_DOWN, ControllerButton::DpadDown);
			MapButton(SDL_GAMEPAD_BUTTON_DPAD_LEFT, ControllerButton::DpadLeft);
			MapButton(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, ControllerButton::DpadRight);
			MapButton(SDL_GAMEPAD_BUTTON_START, ControllerButton::Start);
			MapButton(SDL_GAMEPAD_BUTTON_BACK, ControllerButton::Back);
			MapButton(SDL_GAMEPAD_BUTTON_LEFT_STICK, ControllerButton::LeftThumb);
			MapButton(SDL_GAMEPAD_BUTTON_RIGHT_STICK, ControllerButton::RightThumb);
			MapButton(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, ControllerButton::LeftShoulder);
			MapButton(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, ControllerButton::RightShoulder);
			MapButton(SDL_GAMEPAD_BUTTON_SOUTH, ControllerButton::A);
			MapButton(SDL_GAMEPAD_BUTTON_EAST, ControllerButton::B);
			MapButton(SDL_GAMEPAD_BUTTON_WEST, ControllerButton::X);
			MapButton(SDL_GAMEPAD_BUTTON_NORTH, ControllerButton::Y);

			uint32_t buttonChanges{ m_SDLButtonsCurrent ^ m_SDLButtonsPrevious };
			m_ButtonsPressedThisFrame = buttonChanges & m_SDLButtonsCurrent;
			m_ButtonsReleasedThisFrame = buttonChanges & (~m_SDLButtonsCurrent);
	#endif
		}

		bool InputManagerImpl::IsKeyDown(SDL_Scancode) const
		{
			return false;
		}

		bool InputManagerImpl::IsButtonDownThisFrame(unsigned int button) const noexcept
		{
			return m_ButtonsPressedThisFrame & button;
		}

		bool InputManagerImpl::IsButtonUpThisFrame(unsigned int button) const noexcept
		{
			return m_ButtonsReleasedThisFrame & button;
		}

		bool InputManagerImpl::IsControllerConnected() const noexcept
		{
			return m_ControllerConnected;
		}

		bool InputManagerImpl::IsButtonPressed(unsigned int button) const noexcept
		{
	#ifdef _WIN32
			return m_CurrentState.Gamepad.wButtons & button;
	#else
			return m_SDLButtonsCurrent & button;
	#endif
		}

		glm::vec2 InputManagerImpl::GetLeftStick() const
		{
	#ifdef _WIN32
			// should clamp between -1.f : 1.f for safety is using in animation or physics
			float x{ m_CurrentState.Gamepad.sThumbLX / STICK_MAX_VALUE };
			float y{ m_CurrentState.Gamepad.sThumbLY / STICK_MAX_VALUE };
	#else
			if (!m_pGamepad) 
				return { 0.f, 0.f };

			float x{ SDL_GetGamepadAxis(m_pGamepad, SDL_GAMEPAD_AXIS_LEFTX) / STICK_MAX_VALUE };
			float y{ SDL_GetGamepadAxis(m_pGamepad, SDL_GAMEPAD_AXIS_LEFTY) / STICK_MAX_VALUE };
	#endif // WIN32

			ApplyRadialDeadzone(x, y, STICK_DEADZONE);
			return { x, y };
		}

		glm::vec2 InputManagerImpl::GetRightStick() const
		{
	#ifdef _WIN32
			// should clamp between -1.f : 1.f for safety is using in animation or physics
			float x{ m_CurrentState.Gamepad.sThumbRX / STICK_MAX_VALUE };
			float y{ m_CurrentState.Gamepad.sThumbRY / STICK_MAX_VALUE };
	#else
			if (!m_pGamepad) return { 0.f, 0.f };
			float x{ SDL_GetGamepadAxis(m_pGamepad, SDL_GAMEPAD_AXIS_RIGHTX) / STICK_MAX_VALUE };
			float y{ SDL_GetGamepadAxis(m_pGamepad, SDL_GAMEPAD_AXIS_RIGHTY) / STICK_MAX_VALUE };
	#endif // WIN32

			ApplyRadialDeadzone(x, y, STICK_DEADZONE);
			return { x, y };
		}

		void InputManagerImpl::BindKeyboardCommand(SDL_Scancode key, InputManager::InputTrigger trigger, std::unique_ptr<GameObjectCommand> command)
		{
			m_KeyboardBindings.push_back(KeyBoardBinding{ key, trigger, std::move(command) });
		}

		void InputManagerImpl::BindControllerCommand(unsigned int button, InputManager::InputTrigger trigger, std::unique_ptr<GameObjectCommand> command)
		{
			m_ControllerBindings.push_back(ControllerBinding{ button, trigger, std::move(command) });
		}

		void InputManagerImpl::BindControllerStickCommand(std::unique_ptr<GameObjectCommand> command)
		{
			m_LeftStickCommand = std::move(command);
		}

		void InputManagerImpl::UnbindAll()
		{
			m_KeyboardBindings.clear();
			m_ControllerBindings.clear();
			m_LeftStickCommand.reset();
		}

		void InputManagerImpl::UnbindAllKeyboard()
		{
			m_KeyboardBindings.clear();
		}

		void InputManagerImpl::UnbindAllController()
		{
			m_ControllerBindings.clear();
			m_LeftStickCommand.reset();
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

			// Unbind Controller Stick
			if (m_LeftStickCommand)
			{
				const auto* command{ m_LeftStickCommand.get() };
				if (command && command->GetCommandTarget() == target)
					m_LeftStickCommand.reset();
			}
		}

		void InputManagerImpl::ApplyRadialDeadzone(float& x, float& y, float deadzone) const
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

	#ifndef _WIN32
		void InputManagerImpl::MapButton(SDL_GamepadButton sdlButton, unsigned int bit)
		{
			if (SDL_GetGamepadButton(m_pGamepad, sdlButton))
				m_SDLButtonsCurrent |= bit;
		}
	#endif // !WIN32
	}