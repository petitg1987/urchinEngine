#ifndef ENGINE_OBSERVABLE_H
#define ENGINE_OBSERVABLE_H

#include <set>
#include <map>

#include "pattern/observer/Observer.h"

namespace urchin
{

	class Observable
	{
		public:
			Observable();
			virtual ~Observable();

			void addObserver(Observer *, int);
			void removeObserver(Observer *, int);
			void notifyObservers(Observable *, int);

		private:
			std::map<int, std::set<Observer *> *> mapObservers;
	};

}

#endif
