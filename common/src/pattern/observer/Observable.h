#ifndef URCHINENGINE_OBSERVABLE_H
#define URCHINENGINE_OBSERVABLE_H

#include <unordered_set>
#include <map>

#include "pattern/observer/Observer.h"

namespace urchin {

    class Observable {
        public:
            virtual ~Observable();

            void addObserver(Observer*, int) const;
            void removeObserver(Observer*, int) const;

            void notifyObservers(Observable*, int);

        private:
            mutable std::map<int, std::unordered_set<Observer *> *> mapObservers;
    };

}

#endif
