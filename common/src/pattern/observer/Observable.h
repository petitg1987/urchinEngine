#pragma once

#include <vector>
#include <map>

#include "pattern/observer/Observer.h"

namespace urchin {

    class Observable {
        public:
            virtual ~Observable();

            void addObserver(Observer*, int);
            void removeObserver(Observer*, int);

            void notifyObservers(Observable*, int);

        private:
            std::map<int, std::vector<Observer*>> mapObservers;
    };

}
