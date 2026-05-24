#include "AnimatorComponent.h"
#include "Animation.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform.h"
#include "EngineEvents.h"
#include "Image.h"
#include "SpriteSheet.h"
#include "Renderer.h"

#include <cassert>
#include <utility>


ge::AnimatorComponent::AnimatorComponent(GameObject* ownerPtr, const SpriteSheet* spriteSheet)
	:Component::Component(ownerPtr),
	m_CachedSpriteSheet{ spriteSheet }
{
	assert(!GetOwner()->GetComponent<ge::Image>() 
		&& "Image component cannot exist on the same object as Animator. Choose one.");
}

void ge::AnimatorComponent::UpdateComponent(float deltaTime)
{
	if (!m_CurrentAnimation)
		return;

	if (m_AnimationFinished) // for non-looping animation
		return;

	const float constantFrameAnimTime{ 1.f / m_CurrentAnimation->framesPerSecond };
	m_FrameTimer += deltaTime;

	if (m_FrameTimer > constantFrameAnimTime)
	{
		++m_CurrentFrameIdx;
		if (m_CurrentFrameIdx >= static_cast<int>(m_CurrentAnimation->frames.size()))
		{
			// Loop:
			if (m_CurrentAnimation->loops)
			{
				m_CurrentFrameIdx = 0;
			}
			else // Exit Loop:
			{
				m_AnimationFinished = true;

				m_CurrentFrameIdx = static_cast<int>(m_CurrentAnimation->frames.size() - 1);
				m_FrameTimer = 0.f;
				m_OnAnimationFinishedEvent.NotifyObservers(EngineEventId::ANIMATION_FINISHED, GetOwner());

				return;
			}
		}

		m_FrameTimer -= constantFrameAnimTime;
	}
}

void ge::AnimatorComponent::RenderComponent() const
{
	if (!m_CurrentAnimation || !m_CachedSpriteSheet)
		return;

	// Scaling + anchor Calculations:
	auto pOwnerTransform{ GetOwner()->GetComponent<Transform>() };
	const auto worldPos{ pOwnerTransform->GetWorldPosition() };
	const auto worldScale{ pOwnerTransform->GetWorldScale() };

	const float frameW{ m_CachedSpriteSheet->GetFrameWidth() };
	const float frameH{ m_CachedSpriteSheet->GetFrameHeight() };
	const glm::vec2 scaledSize{ frameW * worldScale.x, frameH * worldScale.y };

	const float topLeftX{ worldPos.x - m_Anchor.x * scaledSize.x };
	const float topLeftY{ worldPos.y - m_Anchor.y * scaledSize.y };

	const auto srcRect{ m_CachedSpriteSheet->GetFrameRect(m_CurrentAnimation->frames[m_CurrentFrameIdx]) };
	// Has to use Renderer
	ge::Renderer::GetInstance().RenderTextureRegion(
		*m_CachedSpriteSheet->GetTexture(), srcRect,
		topLeftX, topLeftY, scaledSize.x, scaledSize.y);
}

void ge::AnimatorComponent::AddAnimation(Animation anim)
{
	assert(!anim.name.empty() && "Animation needs a name");
	m_Animations.try_emplace(anim.name, std::move(anim));
}

void ge::AnimatorComponent::Play(const std::string& name)
{
	// Animation already playing
	if (m_CurrentAnimation && m_CurrentAnimation->name == name)
	{
		return;
	}
		
	// Unknown animation
	auto it{ m_Animations.find(name) };
	if (it == m_Animations.end())
	{
		assert("Unknown animation!");
		return;
	}

	m_CurrentAnimation = &it->second;
	// RESET:
	m_CurrentFrameIdx = 0;
	m_FrameTimer = 0.f;	
	m_AnimationFinished = false;
}

void ge::AnimatorComponent::Stop()
{
	m_CurrentAnimation = nullptr;
}

ge::Subject& ge::AnimatorComponent::GetOnAnimationFinishedEvent() noexcept
{
	return m_OnAnimationFinishedEvent;
}

void ge::AnimatorComponent::SetAnchor(const glm::vec2& anchor) noexcept
{
	m_Anchor = anchor;
}

const glm::vec2& ge::AnimatorComponent::GetAnchor() const noexcept
{
	return m_Anchor;
}

glm::vec2 ge::AnimatorComponent::GetSingleFrameRectSize() const noexcept
{
	return glm::vec2{ m_CachedSpriteSheet->GetFrameWidth(), m_CachedSpriteSheet->GetFrameHeight() };
}
