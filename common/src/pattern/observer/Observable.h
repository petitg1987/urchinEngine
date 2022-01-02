#pragma once

#include <vector>
#include <map>
#include <memory>

#include <pattern/observer/Observer.h>

namespace urchin {

    class Observable {
        public:
            virtual ~Observable();

            void addObserver(Observer*, int) const; //TODO remove const !
            void removeObserver(Observer*, int) const;

            void notifyObservers(Observable*, int);

        private:
            mutable std::map<int, std::vector<Observer*>> mapObservers;
    };

}
