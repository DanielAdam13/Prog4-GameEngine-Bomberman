#include "ChangeWindowSizeCommand.h"
#include "Renderer.h"

ge::ChangeWindowSizeCommand::ChangeWindowSizeCommand(const int changeSizeX, const int changeSizeY)
	:m_OriginalSize{ Renderer::GetInstance().GetWindowSize().first, Renderer::GetInstance().GetWindowSize().second },
	m_ChangedSize{ changeSizeX, changeSizeY },
	m_IsInChanged{ false }
{
}

void ge::ChangeWindowSizeCommand::Execute(float)
{
	Renderer::GetInstance().SetWindowSize((m_IsInChanged) ? m_OriginalSize : m_ChangedSize);
	m_IsInChanged = !m_IsInChanged;
}
