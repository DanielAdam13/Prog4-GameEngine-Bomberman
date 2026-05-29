#include "BreakableWallComponent.h"
#include "GameObject.h"
#include "Components/AnimatorComponent.h"

#include <cassert>

bombGame::BreakableWallComponent::BreakableWallComponent(ge::GameObject* owner, float crumblingDuration)
	:Component::Component(owner),
	m_CrumblingDuration{ crumblingDuration },
	m_CachedAnimatorComp{ owner->GetComponent<ge::AnimatorComponent>() }
{
	assert(m_CachedAnimatorComp && "BreakableWall Component requires an Animator on the same GameObject");
}

void bombGame::BreakableWallComponent::UpdateComponent(float deltaTime)
{
	if (m_CrumbleFlag)
	{
		m_CrumblingTimer += deltaTime;
		if (m_CrumblingTimer >= m_CrumblingDuration)
		{
			m_CrumblingTimer = 0.f;

			GetOwner()->MarkForDeletion();
		}
	}
}

void bombGame::BreakableWallComponent::Crumble() noexcept
{
	m_CrumbleFlag = true;
	m_CachedAnimatorComp->Play("crumbling");
}
