#pragma once
#include "Component.h"

namespace ge
{
	class TextComponent;

	class FPSComponent final : public Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every Image Instance shares the same component type ID
		static constexpr ComponentTypeID StaticTypeID{ 3 };

		FPSComponent() = default;
		~FPSComponent() override = default;

		FPSComponent(const FPSComponent& other) = delete;
		FPSComponent(FPSComponent&& other) = delete;
		FPSComponent& operator=(const FPSComponent& other) = delete;
		FPSComponent& operator=(FPSComponent&& other) = delete;

		virtual void FixedUpdateComponent(float) override {};
		virtual void UpdateComponent(float) override;
		virtual void RenderComponent(const glm::vec3&) const override {};

	private:
		TextComponent* m_pTextComponent{ nullptr };
		float m_ChangeTimer{ 0.f };

		static constexpr float ChangeInterval{ 0.25f };
	};
}