#pragma once

namespace ge
{
	class Command
	{
	public:
		Command() = default;
		virtual ~Command() = default;

		virtual void Execute(float) = 0;

	protected:

	private:

	};
}