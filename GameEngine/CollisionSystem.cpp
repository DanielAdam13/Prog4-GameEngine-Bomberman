#include "CollisionSystem.h"

#include "GameObject.h"
#include "Components/Colliders.h"

#include <vector>
#include <algorithm>

void ge::CollisionSystem::Register(Collider* coll)
{
	m_Colliders.push_back(coll);
}

void ge::CollisionSystem::Unregister(Collider* coll)
{
    auto it{ std::find(m_Colliders.begin(), m_Colliders.end(), coll) };

    if (it != m_Colliders.end())
        m_Colliders.erase(it);
}

void ge::CollisionSystem::UnregisterAll()
{
    m_Colliders.clear();
}

void ge::CollisionSystem::UpdateCollision()
{
    // 0. Don't care about invalid colliders
    std::erase_if(m_Colliders, [](Collider* c) {
        return !c || c->GetOwner()->MarkedForDeletion() || c->MarkedForDeletion();
        });

    // 1. Begin
    for (auto* coll : m_Colliders)
    {
        coll->BeginOverlap();
    }

    // 2. In collision
    for (size_t i{}; i < m_Colliders.size(); ++i)
    {
        for (size_t j{ i + 1 }; j < m_Colliders.size(); ++j)
        {
            Collider* a{ m_Colliders[i] };
            Collider* b{ m_Colliders[j] };

            if (Overlaps(*a, *b))
            {
                a->RecordOverlap(b);
                b->RecordOverlap(a);
            }
        }
    }

    // 3. End
    for (auto* coll : m_Colliders)
    {
        coll->EndOverlap();
    }
}

void ge::CollisionSystem::AddLayerTag(const CollisionLayerTag& layerTag)
{
    m_LayerTags.insert(layerTag);
}

void ge::CollisionSystem::RemoveLayerTag(const CollisionLayerTag& layerTag)
{
    m_LayerTags.erase(layerTag);
}

const std::unordered_set<ge::CollisionLayerTag>& ge::CollisionSystem::GetLayerTags() const noexcept
{
    return m_LayerTags;
}

bool ge::CollisionSystem::AnyOverlapWithTags(const structs::Rect& bounds, std::initializer_list<CollisionLayerTag> tags) const
{
    if (tags.size() == 0)
        return false;

    for (auto* c : m_Colliders)
    {
        // Check if any tag matches 
        const auto& colliderTag{ c->GetLayerTag() };
        const bool tagMatches{ std::any_of(tags.begin(), tags.end(),
            [&colliderTag](const CollisionLayerTag& t)
            {
                return t == colliderTag;
            }) };

        if (!tagMatches)
            continue;

        // Check for overlaps with a tag is matching
        switch (c->GetShape())
        {
        case Collider::Shape::Box:
        {
            auto* box{ static_cast<BoxCollider*>(c) };
            if (collisionHelpers::Overlaps(bounds, box->GetBounds()))
                return true;
            break;
        }
        case Collider::Shape::Circle:
        {
            auto* circle{ static_cast<CircleCollider*>(c) };
            if (collisionHelpers::Overlaps(bounds, circle->GetBounds()))
                return true;
            break;
        }
        }
    }
    return false;
}

bool ge::CollisionSystem::Overlaps(const Collider& a, const Collider& b)
{
    switch (a.GetShape())
    {
    case ge::Collider::Shape::Box:
    {
        const auto& boxCollA{ static_cast<const BoxCollider&>(a) };
        if (b.GetShape() == Collider::Shape::Box)
        {
            return collisionHelpers::Overlaps(boxCollA.GetBounds(),
                static_cast<const BoxCollider&>(b).GetBounds());
        }
        else
        {
            return collisionHelpers::Overlaps(boxCollA.GetBounds(),
                static_cast<const CircleCollider&>(b).GetBounds());
        }
    }
    case ge::Collider::Shape::Circle:
    {
        const auto& circleCollA{ static_cast<const CircleCollider&>(a) };
        if (b.GetShape() == Collider::Shape::Box)
        {
            return collisionHelpers::Overlaps(circleCollA.GetBounds(),
                static_cast<const BoxCollider&>(b).GetBounds());
        }
        else
        {
            return collisionHelpers::Overlaps(circleCollA.GetBounds(),
                static_cast<const CircleCollider&>(b).GetBounds());
        }
    }
    }

    return false;
}
