#include "pattern/observer/Observable.h"

namespace urchin
{

	Observable::Observable()
	{

	}

	Observable::~Observable()
	{
		for(std::map<int, std::set<Observer *> *>::const_iterator it = mapObservers.begin(); it!=mapObservers.end(); ++it)
		{
			delete it->second;
		}
	}

	void Observable::addObserver(Observer *observer, int notificationType) const
	{
		std::map<int, std::set<Observer *> *>::const_iterator it = mapObservers.find(notificationType);
		if(it==mapObservers.end())
		{
			std::set<Observer *> *observers = new std::set<Observer *>();
			observers->insert(observer);
			mapObservers[notificationType] = observers;
		}else
		{
			std::set<Observer *> *observers = it->second;
			observers->insert(observer);
		}
	}

	void Observable::removeObserver(Observer *observer, int notificationType) const
	{
		std::map<int, std::set<Observer *> *>::const_iterator it = mapObservers.find(notificationType);
		if(it!=mapObservers.end())
		{
			std::set<Observer *> *observers = it->second;
			observers->erase(observer);
		}
	}

	/**
	* @param observable Observable producing this notification
	*/
	void Observable::notifyObservers(Observable *observable, int notificationType)
	{
		std::map<int, std::set<Observer *> *>::const_iterator it = mapObservers.find(notificationType);
		if(it!=mapObservers.end())
		{
			std::set<Observer *> *observers = it->second;
			for(std::set<Observer *>::const_iterator itObservers=observers->begin(); itObservers!=observers->end(); ++itObservers)
			{
				(*itObservers)->notify(observable, notificationType);
			}
		}
	}
}
