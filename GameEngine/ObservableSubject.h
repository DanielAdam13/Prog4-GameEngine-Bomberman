#pragma once
#include <vector>
#include "Observer.h"

namespace ge
{
	class GameObject;
}

namespace bombGame
{
	class Subject final
	{
	public:
		explicit Subject(const EventId id);
		~Subject() = default;

		void AddObserver(Observer* observer);
		void RemoveObserver(Observer* observer);

		void NotifyObservers(ge::GameObject* sourceObject);

	private:
		std::vector<Observer*> m_SubjectObservers{};

		const EventId m_EventId{};
	};
}