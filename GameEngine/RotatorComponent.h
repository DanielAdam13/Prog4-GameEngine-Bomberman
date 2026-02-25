#pragma once
#include "Component.h"

namespace ge
{
	class GameObject;
	class Transform;

	class Rotator final : public Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every Rotator Instance shares the same component type ID
		static constexpr ComponentTypeID StaticTypeID{ 4 };

		Rotator(GameObject* pOwnerPtr, bool isClockwise = true, float radius = 50.f, float angularSpeedDegrees = 180.f);
		~Rotator() override = default;

		Rotator(const Rotator& other) = delete;
		Rotator(Rotator&& other) = delete;
		Rotator& operator=(const Rotator& other) = delete;
		Rotator& operator=(Rotator&& other) = delete;

		virtual void FixedUpdateComponent(float) override {};
		virtual void UpdateComponent(float) override;
		virtual void RenderComponent() const override {};

	private:
		Transform* pOwnerTransform; // Doesn't own
		bool m_ClockwiseRotation;

		const glm::vec3 m_CircleCenter;
		const float m_Radius;
		const float m_AngularSpeed;

		float m_Angle;

	};
}