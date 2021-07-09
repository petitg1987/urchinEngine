#pragma once

#include <unordered_set>
#include <map>
#include <memory>

#include <pattern/observer/Observer.h>

namespace urchin {

    class Observable {
        public:
            virtual ~Observable();

            void addObserver(Observer*, int) const;
            void removeObserver(Observer*, int) const;

            void notifyObservers(Observable*, int);

        private:
            mutable std::map<int, std::unique_ptr<std::unordered_set<Observer*>>> mapObservers;
    };

}
