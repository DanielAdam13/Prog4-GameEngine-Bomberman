#pragma once
#include "Components/Component.h"

#include <vector>
#include <glm/glm.hpp>

namespace ge
{
	class Camera;
	class GameObject;
}

namespace bombGame
{
	class CameraPlayerFollowComponent final : public ge::Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every Anomator Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 21 };

		CameraPlayerFollowComponent(ge::GameObject* ownerPtr, ge::Camera* camera, float smoothFactor = 5.f);
		~CameraPlayerFollowComponent() override = default;
		CameraPlayerFollowComponent(const CameraPlayerFollowComponent& other) = delete;
		CameraPlayerFollowComponent(CameraPlayerFollowComponent&& other) = delete;
		CameraPlayerFollowComponent& operator=(const CameraPlayerFollowComponent& other) = delete;
		CameraPlayerFollowComponent& operator=(CameraPlayerFollowComponent&& other) = delete;

		void AddTarget(ge::GameObject* newTarget);

		virtual void FixedUpdateComponent(float) override {};
		virtual void UpdateComponent(float deltaTime) override;
		virtual void RenderComponent() const override {};

	private:
		ge::Camera* m_Camera; // Non-owning reference
		std::vector<ge::GameObject*> m_Targets{}; // Non-owning references to players

		float m_SmoothFactor;
		glm::vec2 ComputeAverageTargetPosition() const noexcept;
	};
}