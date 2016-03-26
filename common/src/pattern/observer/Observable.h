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

			void addObserver(Observer *, int) const;
			void removeObserver(Observer *, int) const;
			void notifyObservers(Observable *, int);

		private:
			mutable std::map<int, std::set<Observer *> *> mapObservers;
	};

}

#endif
