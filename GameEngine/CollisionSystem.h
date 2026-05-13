#pragma once
#include "Singleton.h"
#include "Components/Colliders.h"
#include "Structs.h"

#include <vector>
#include <string>
#include <unordered_set>
#include <initializer_list>

namespace ge
{
	// Static singleton class MANAGING all registered Colliders
	class CollisionSystem final : public Singleton<CollisionSystem>
	{
	public:
		void Register(Collider* coll);
		void Unregister(Collider* coll);
		void UnregisterAll();

		void UpdateCollision();

		void AddLayerTag(const CollisionLayerTag& layerTag);
		void RemoveLayerTag(const CollisionLayerTag& layerTag);

		const std::unordered_set<CollisionLayerTag>& GetLayerTags() const noexcept;

		// Making use initializer list!!!
		bool AnyOverlapWithTags(const structs::Rect& bounds, std::initializer_list<CollisionLayerTag> tags) const;

	private:
		std::vector<Collider*> m_Colliders;
		std::unordered_set<CollisionLayerTag> m_LayerTags{ "Default" };

		bool Overlaps(const Collider& a, const Collider& b);
	};
}