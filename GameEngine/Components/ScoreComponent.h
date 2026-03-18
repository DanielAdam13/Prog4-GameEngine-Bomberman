#pragma once
#include "Component.h"

namespace bombGame
{
	// Game-side Component
	class ScoreComponent final : public ge::Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every ScoreComponent Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 6 };

		ScoreComponent(ge::GameObject* pOwnerPtr, int startingScore);
		virtual ~ScoreComponent() override = default;

		virtual void RenderComponent() const override {};
		virtual void FixedUpdateComponent(float) override {};
		virtual void UpdateComponent(float) override {};

		void ChangeScore(int scoreDiff);
		int GetCurrentScore() const noexcept;

		void SetOnScoreChange(std::function<void()> callback);

	private:
		int m_CurrentScore{};

		std::function<void()> m_OnScoreChange{};
	};
}