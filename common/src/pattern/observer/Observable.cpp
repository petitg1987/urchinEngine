#include <algorithm>

#include "pattern/observer/Observable.h"

namespace urchin {

    Observable::Observable() :
            notifying(false) {

    }

    Observable::~Observable() {
        mapObservers.clear();
    }

    void Observable::addObserver(Observer* observer, int notificationType) {
        auto itFind = mapObservers.find(notificationType);
        if (itFind != mapObservers.end()) {
            if (std::ranges::find(itFind->second, observer) == itFind->second.end()) {
                itFind->second.push_back(observer);
            }
        } else {
            std::vector observers = {observer};
            mapObservers.try_emplace(notificationType, observers);
        }
    }

    void Observable::addObserverAndTriggerNotify(Observer* observer, int notificationType) {
        addObserver(observer, notificationType);
        notifyObservers(this, notificationType);
    }

    void Observable::removeObserver(Observer* observer, int notificationType) {
        auto itObservers = mapObservers.find(notificationType);
        if (itObservers != mapObservers.end()) {
            if (notifying) {
                auto itObserver = std::ranges::find(itObservers->second, observer);
                if (itObserver != itObservers->second.end()) {
                    *itObserver = nullptr;
                }
            } else {
                std::erase(itObservers->second, observer);
            }
        }
    }

    std::span<Observer* const> Observable::getObservers(int notificationType) const {
        auto itObservers = mapObservers.find(notificationType);
        if (itObservers != mapObservers.end()) {
            return itObservers->second;
        }
        return {};
    }

    /**
    * @param observable Observable producing this notification
    */
    void Observable::notifyObservers(Observable* observable, int notificationType) {
        auto itFind = mapObservers.find(notificationType);
        if (itFind != mapObservers.end()) {
            notifying = true;
            std::size_t count = itFind->second.size();
            for (std::size_t i = 0; i < count; i++) {
                if (itFind->second[i]) {
                    itFind->second[i]->notify(observable, notificationType);
                }
            }
            notifying = false;
            std::erase(itFind->second, nullptr);
        }
    }
}
