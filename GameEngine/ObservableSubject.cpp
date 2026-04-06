#include "ObservableSubject.h"
#include "GameObject.h"

using namespace ge;

Subject::Subject()
{
}

void Subject::AddObserver(IObserver* observer)
{
	m_SubjectObservers.push_back(observer);
}

void Subject::RemoveObserver(IObserver* observer)
{
	m_SubjectObservers.erase(
		std::remove(m_SubjectObservers.begin(), m_SubjectObservers.end(), observer),
		m_SubjectObservers.end()
	);
}

void Subject::NotifyObservers(int eventId, ge::GameObject* sourceObject)
{
	for (auto* observer : m_SubjectObservers)
	{
		observer->Notify(eventId, sourceObject);
	}
}
