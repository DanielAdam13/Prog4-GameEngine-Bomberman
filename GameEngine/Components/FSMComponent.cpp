#include "FSMComponent.h"
#include <utility>

ge::FSMComponent::FSMComponent(GameObject* pOwnerPtr)
	:Component::Component(pOwnerPtr)
{
}

void ge::FSMComponent::UpdateComponent(float deltaTime)
{
	if (!m_CurrentState)
		return;

	m_CurrentState->OnUpdate(deltaTime);

	auto currentStateIt{ m_TransitionsContainer.find(m_CurrentState) };
	if (currentStateIt == m_TransitionsContainer.end())
		return;

	// Only the first EVALUATED transition will be used, meaning there is a priority concern
	for (const auto& t : currentStateIt->second)
	{
		if (t.Evaluate())
		{
			// Change to new state via INFORMATION from the transition
			ChangeState(t.GetTo());
			break;
		}
	}
}

void ge::FSMComponent::Start()
{
	if (m_IsRunning || m_StatesContainer.empty())
		return;

	m_IsRunning = true;

	ChangeState(m_StatesContainer.front().get());
}

void ge::FSMComponent::Stop()
{
	if (!m_IsRunning)
		return;

	if (m_CurrentState)
		m_CurrentState->OnExit();

	m_CurrentState = nullptr;
	m_IsRunning = false;
}

ge::State* ge::FSMComponent::AddState(std::unique_ptr<State>&& newState)
{
	if (!newState)
		return nullptr;

	State* rawState{ newState.get() };
	m_StatesContainer.push_back(std::move(newState));
	return rawState;
}

void ge::FSMComponent::AddTransition(State* from, State* to, std::function<bool()> evalFunc)
{
	if (!from || !to || !evalFunc)
		return;

	m_TransitionsContainer[from].emplace_back(from, to, std::move(evalFunc));
}

void ge::FSMComponent::ChangeState(State* newState)
{
	if (m_CurrentState == newState)
		return;

	// 1.
	if (m_CurrentState)
		m_CurrentState->OnExit();

	// 2.
	m_CurrentState = newState;

	// 3.
	m_CurrentState->OnEnter();
}
