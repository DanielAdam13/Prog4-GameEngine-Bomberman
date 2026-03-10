#pragma once

namespace ge
{
	class ImGuiInstance
	{
	public:		
		explicit ImGuiInstance() = default;
		virtual ~ImGuiInstance() = default;
		virtual void RenderImGui() const = 0;
		virtual void UpdateImGui(float) = 0;

	private:
		
	};

	class Exercise1ImGui final : public ImGuiInstance
	{
	public:
		explicit Exercise1ImGui() = default;
		virtual ~Exercise1ImGui() override = default;

		virtual void RenderImGui() const override;
		virtual void UpdateImGui(float) override;

	private:

	};

	class Exercise2ImGui final : public ImGuiInstance
	{
	public:
		explicit Exercise2ImGui() = default;
		virtual ~Exercise2ImGui() override = default;

		virtual void RenderImGui() const override;
		virtual void UpdateImGui(float) override;

	private:

	};
}