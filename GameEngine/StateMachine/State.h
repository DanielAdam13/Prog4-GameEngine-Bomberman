#pragma once

namespace ge
{
	class GameObject;

	class State
	{
	public:
		State(GameObject* pTargetPtr);
		virtual ~State() = default;
		State(const State&) = delete;
		State& operator=(const State&) = delete;
		State(State&&) = delete;
		State& operator=(State&&) = delete;

		virtual void OnEnter() = 0;
		virtual void OnUpdate(float) = 0;
		virtual void OnExit() = 0;

	protected:
		GameObject* GetSource() const noexcept;

	private:
		GameObject* m_TargetPtr{ nullptr };

	};
}