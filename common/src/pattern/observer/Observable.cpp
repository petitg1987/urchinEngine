#include <pattern/observer/Observable.h>

namespace urchin {

    Observable::~Observable() {
        mapObservers.clear();
    }

    void Observable::addObserver(Observer* observer, int notificationType) const {
        auto it = mapObservers.find(notificationType);
        if (it == mapObservers.end()) {
            auto observers = std::make_unique<std::unordered_set<Observer*>>();
            observers->insert(observer);
            mapObservers[notificationType] = std::move(observers);
        } else {
            it->second->insert(observer);
        }
    }

    void Observable::removeObserver(Observer* observer, int notificationType) const {
        auto it = mapObservers.find(notificationType);
        if (it != mapObservers.end()) {
            const auto& observers = it->second;
            observers->erase(observer);
        }
    }

    /**
    * @param observable Observable producing this notification
    */
    void Observable::notifyObservers(Observable* observable, int notificationType) {
        auto it = mapObservers.find(notificationType);
        if (it != mapObservers.end()) {
            for (auto observer : *it->second) {
                observer->notify(observable, notificationType);
            }
        }
    }
}
