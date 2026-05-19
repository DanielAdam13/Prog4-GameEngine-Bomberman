#pragma once
#include "Component.h"
#include "ObservableSubject.h"

#include <string>
#include <unordered_map>

namespace ge
{
	struct Animation;
	class SpriteSheet;
	class GameObject;

	class AnimatorComponent final : public Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every Anomator Instance shares the same component type ID
		static constexpr ComponentTypeID StaticTypeID{ 17 };

		AnimatorComponent(GameObject* ownerPtr, const SpriteSheet* spriteSheet);
		~AnimatorComponent() override = default;
		AnimatorComponent(const AnimatorComponent& other) = delete;
		AnimatorComponent(AnimatorComponent&& other) = delete;
		AnimatorComponent& operator=(const AnimatorComponent& other) = delete;
		AnimatorComponent& operator=(AnimatorComponent&& other) = delete;

		virtual void FixedUpdateComponent(float) override {};
		virtual void UpdateComponent(float deltaTime) override;
		virtual void RenderComponent() const override;

		void AddAnimation(Animation anim);
		void Play(const std::string& name);
		void Stop();

		Subject& GetOnAnimationFinishedEvent() noexcept;

		void SetAnchor(const glm::vec2& anchor) noexcept;
		const glm::vec2& GetAnchor() const noexcept;

		glm::vec2 GetSingleFrameRectSize() const noexcept;

	private:
		const SpriteSheet* m_CachedSpriteSheet; // Non-owning, this is a resource managed by ResourceManager

		std::unordered_map<std::string, Animation> m_Animations{};
		Animation* m_CurrentAnimation{ nullptr };

		int m_CurrentFrameIdx{ 0 };
		float m_FrameTimer{ 0.f };
		bool m_AnimationFinished{ false };

		glm::vec2 m_Anchor{ 0.f, 0.f };

		Subject m_OnAnimationFinishedEvent;
	};
}