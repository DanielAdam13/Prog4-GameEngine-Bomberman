#pragma once

namespace ge
{
	// Interface exposing what would happen OnSelection, OnExit
	class ISelectable
	{
	public:
		virtual ~ISelectable() = default;

		virtual void OnSelected() = 0;
		virtual void OnHover() = 0;
		virtual void OnExit() = 0;

	protected:

	private:
	};
}