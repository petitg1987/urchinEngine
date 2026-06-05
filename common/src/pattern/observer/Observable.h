#pragma once

#include <vector>
#include <map>
#include <span>

#include "pattern/observer/Observer.h"

namespace urchin {

    class Observable {
        public:
            Observable();
            virtual ~Observable();

            void addObserver(Observer*, int);
            void addObserverAndTriggerNotify(Observer*, int);
            void removeObserver(Observer*, int);
            std::span<Observer* const> getObservers(int) const;

            void notifyObservers(Observable*, int);

        private:
            bool notifying;
            std::map<int, std::vector<Observer*>> mapObservers;
    };

}
