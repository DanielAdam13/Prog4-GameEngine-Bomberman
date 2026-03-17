#pragma once
#include "Observer.h"

namespace bombGame
{
	class Achievements final : public Observer
	{
	public:
		Achievements() = default;
		virtual ~Achievements() override = default;

		//virtual void Notify(Event event, GameActor*) override;

	private:

	};
}