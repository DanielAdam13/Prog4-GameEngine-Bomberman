#include "ImGuiScene.h"
#include <imgui.h>
#include <chrono>
#include <algorithm>
#include <numeric>

using namespace ge;

Exercise1ImGui::Exercise1ImGui()
	:m_LargeBuffer{},
	m_AveragedTimings{},
	m_StepValues{},
	m_SamplesNr{ 10 }
{
	m_LargeBuffer.resize(1 << 26, 1);
	//RunBenchmark();
}

void Exercise1ImGui::RenderImGui()
{
	ImGui::Begin("Exerice 1");

	ImGui::Text("Samples:");
	ImGui::SameLine();

	ImGui::SetNextItemWidth(60.f);
	if (ImGui::InputInt("##samples", &m_SamplesNr, 0, 0))
		m_SamplesNr = std::max(3, m_SamplesNr); // min 3

	ImGui::SameLine();
	if (ImGui::Button("-"))
	{
		m_SamplesNr = std::max(3, m_SamplesNr - 1);
	}
	ImGui::SameLine();
	if (ImGui::Button("+"))
	{
		++m_SamplesNr;
	}

	if (ImGui::Button("Thrash the Cache"))
		RunBenchmark();

	if (!m_AveragedTimings.empty())
	{
		const float maxVal{ *std::max_element(m_AveragedTimings.begin(), m_AveragedTimings.end()) };

		ImGui::PlotLines(
			"##cache",
			m_AveragedTimings.data(),
			static_cast<int>(m_AveragedTimings.size()),
			0,
			nullptr,
			0.f,
			maxVal * 1.2f,
			ImVec2(0, 150) );
	}

	ImGui::End();
}

void Exercise1ImGui::RunBenchmark()
{
	// Clear Buffers
	m_AveragedTimings.clear();
	m_StepValues.clear();

	// Temp vector to store samples
	std::vector<float> stepSamples;
	stepSamples.reserve(m_SamplesNr);

	// Steps loop
	for (int step{ MinStep }; step <= MaxStep; step *= 2)
	{
		stepSamples.clear();

		// Take SampleNr samples
		for (int s{}; s < m_SamplesNr; ++s)
		{
			// Large buffer fill initalizing 
			std::fill(m_LargeBuffer.begin(), m_LargeBuffer.end(), 1);

			const auto startTime{ std::chrono::high_resolution_clock::now() };
			// Make the calculation
			for (size_t i{}; i < m_LargeBuffer.size(); i += step)
			{
				m_LargeBuffer[i] *= 2;
			}
			const auto endTime{ std::chrono::high_resolution_clock::now() };
			const float duration{ std::chrono::duration<float, std::milli>(endTime - startTime).count() };
			stepSamples.push_back(duration);
		}

		if (stepSamples.size() > 2)
		{
			const auto minEl{ std::min_element(stepSamples.begin(), stepSamples.end()) };
			const auto maxEl{ std::max_element(stepSamples.begin(), stepSamples.end()) };

			if (minEl < maxEl)
			{
				stepSamples.erase(maxEl);
				stepSamples.erase(minEl);
			}
			else
			{
				stepSamples.erase(minEl);
				stepSamples.erase(maxEl);
			}
		}

		const float average{ std::accumulate(stepSamples.begin(), stepSamples.end(), 0.f) /
			static_cast<float>(stepSamples.size()) };

		m_AveragedTimings.push_back(average);
		m_StepValues.push_back(static_cast<float>(step));
	}
}

Exercise2ImGui::Exercise2ImGui()
{

}

void Exercise2ImGui::RenderImGui()
{
	ImGui::Begin("Exerice 2");

	ImGui::End();
}