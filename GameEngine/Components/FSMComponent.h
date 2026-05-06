#pragma once
#include "Component.h"
#include "StateMachine/State.h" // needed for vector type size
#include "StateMachine/Transition.h" // needed for unordered map type size

#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

namespace ge
{
	class FSMComponent final : public Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every FSMComponent Instance shares the same component type ID
		static constexpr ComponentTypeID StaticTypeID{ 10 };

		FSMComponent(GameObject* pOwnerPtr);
		~FSMComponent() override = default;
		FSMComponent(const FSMComponent& other) = delete;
		FSMComponent(FSMComponent&& other) = delete;
		FSMComponent& operator=(const FSMComponent& other) = delete;
		FSMComponent& operator=(FSMComponent&& other) = delete;

		virtual void FixedUpdateComponent(float) override {};
		virtual void UpdateComponent(float) override;
		virtual void RenderComponent() const override {};

		void Start();
		void Stop();

		// Rvalue ref to a unique pointer to SHOW ownership transfer
		State* AddState(std::unique_ptr<State>&& newState);
		void AddTransition(State* from, State* to, std::function<bool()> evalFunc);

	private:
		std::vector<std::unique_ptr<State>> m_StatesContainer{};
		// Store transition with a State key, no transition whould exists without a state
		std::unordered_map<State*, std::vector<Transition>> m_TransitionsContainer{};

		State* m_CurrentState{ nullptr };

		void ChangeState(State* newState);

		bool m_IsRunning{ false };

	};
}