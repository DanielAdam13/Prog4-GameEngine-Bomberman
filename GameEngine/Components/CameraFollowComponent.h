#pragma once
#include "Component.h"

#include <vector>

namespace ge
{
	class Camera;
	class GameObject;
	
	class CameraFollowComponent final : public Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every Anomator Instance shares the same component type ID
		static constexpr ComponentTypeID StaticTypeID{ 42 };

		CameraFollowComponent(GameObject* ownerPtr, Camera* camera, float smoothFactor = 5.f);
		~CameraFollowComponent() override = default;
		CameraFollowComponent(const CameraFollowComponent& other) = delete;
		CameraFollowComponent(CameraFollowComponent&& other) = delete;
		CameraFollowComponent& operator=(const CameraFollowComponent& other) = delete;
		CameraFollowComponent& operator=(CameraFollowComponent&& other) = delete;

		void AddTarget(GameObject* newTarget);

		virtual void FixedUpdateComponent(float) override {};
		virtual void UpdateComponent(float deltaTime) override;
		virtual void RenderComponent() const override {};

	private:
		Camera* m_Camera; // Not-owning reference
		std::vector<GameObject*> m_Targets{}; // Not-owning references

		float m_SmoothFactor;
		glm::vec2 ComputeAverageTargetPosition() const noexcept;
	};
}