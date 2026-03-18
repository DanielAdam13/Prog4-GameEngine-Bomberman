#include "ObservableSubject.h"
#include "GameObject.h"

using namespace bombGame;

Subject::Subject(EventId id)
	:m_EventId{ id }
{
}

void Subject::AddObserver(Observer* observer)
{
	m_SubjectObservers.push_back(observer);
}

void Subject::RemoveObserver(Observer* observer)
{
	m_SubjectObservers.erase(
		std::remove(m_SubjectObservers.begin(), m_SubjectObservers.end(), observer),
		m_SubjectObservers.end()
	);
}

void Subject::NotifyObservers(ge::GameObject* sourceObject)
{
	for (auto* observer : m_SubjectObservers)
	{
		observer->Notify(m_EventId, sourceObject);
	}
}
