#pragma once
#include <vector>
#include <string>

namespace ge
{
	class ImGuiInstance
	{
	public:		
		explicit ImGuiInstance() = default;
		virtual ~ImGuiInstance() = default;
		virtual void RenderImGui() = 0;
		virtual void UpdateImGui(float) = 0;

	private:
		
	};

	class Exercise1ImGui final : public ImGuiInstance
	{
	public:
		explicit Exercise1ImGui();
		virtual ~Exercise1ImGui() override = default;

		virtual void RenderImGui() override;
		virtual void UpdateImGui(float) override {};

	private:
		std::vector<int> m_LargeBuffer;

		std::vector<float> m_AveragedTimings;
		std::vector<float> m_StepValues;
		
		int m_SamplesNr;
		static constexpr int MinStep{ 2 };
		static constexpr int MaxStep{ 1024 };
		
		void RunBenchmark();
		
	};

	class Exercise2ImGui final : public ImGuiInstance
	{
	public:
		explicit Exercise2ImGui();
		virtual ~Exercise2ImGui() override = default;

		virtual void RenderImGui() override;
		virtual void UpdateImGui(float) override {};

	private:

	};
}