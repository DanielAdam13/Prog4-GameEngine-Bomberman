#pragma once
#include "Component.h"
#include "ObservableSubject.h"

namespace ge
{
	class GameObject;
	class TextComponent;

	// Engine Component
	class TimerDisplayComponent final : public Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every Timer Instance shares the same component type ID
		static constexpr ComponentTypeID StaticTypeID{ 43 };

		enum class TimerType
		{
			Incrementing,
			Decrementing
		};

		TimerDisplayComponent(GameObject* pOwnerPtr, float startTime, float goalTime, TimerType type = TimerType::Incrementing, float changeInterval = 0.25f);
		~TimerDisplayComponent() override = default;
		TimerDisplayComponent(const TimerDisplayComponent& other) = delete;
		TimerDisplayComponent(TimerDisplayComponent&& other) = delete;
		TimerDisplayComponent& operator=(const TimerDisplayComponent& other) = delete;
		TimerDisplayComponent& operator=(TimerDisplayComponent&& other) = delete;

		virtual void FixedUpdateComponent(float) override {};
		virtual void UpdateComponent(float) override;
		virtual void RenderComponent() const override {};

		float GetActualTimer() const noexcept { return m_ActualTimer; }
		Subject& GetReachedGoalEvent() noexcept { return m_ReachedGoalEvent; }

	private:
		TextComponent* m_pTextComponent{ nullptr }; // Cached ref from owner
		float m_ActualTimer;
		const float m_GoalTime;
		float m_ChangeTimer;
		const float m_ChangeInterval;
		const TimerType m_TimerType;

		bool m_ReachedGoal{ false };
		Subject m_ReachedGoalEvent{};
	};
}