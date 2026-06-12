#pragma once
#include "Component.h"
#include "ObservableSubject.h"

namespace ge
{
	// Engine Component
	class ScoreComponent final : public ge::Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every ScoreComponent Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 44 };

		ScoreComponent(ge::GameObject* pOwnerPtr, int startingScore);
		virtual ~ScoreComponent() override = default;

		virtual void RenderComponent() const override {};
		virtual void FixedUpdateComponent(float) override {};
		virtual void UpdateComponent(float) override {};

		void ChangeScore(int scoreDiff);
		int GetCurrentScore() const noexcept;

		Subject& GetOnScoreChangedEvent() noexcept { return m_OnScoreChangedEvent; };

	private:
		int m_CurrentScore{};

		Subject m_OnScoreChangedEvent;
	};
}