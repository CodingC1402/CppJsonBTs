#include "ObservePattern.h"

Observer::~Observer()
{
	_oberserving->UnSubscribe(this);
}

void Observable::UnSubscribe(Observer* observer)
{
	_observers.remove(observer);
}

void Observable::Subscribe(Observer* observer)
{
	_observers.push_back(observer);
}

void Observable::CallOnChange()
{
	for (const auto& observer : _observers)
	{
		observer->OnChange();
	}
}
