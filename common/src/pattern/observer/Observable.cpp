#include <pattern/observer/Observable.h>
#include <algorithm>

namespace urchin {

    Observable::~Observable() {
        mapObservers.clear();
    }

    void Observable::addObserver(Observer* observer, int notificationType) const {
        auto itFind = mapObservers.find(notificationType);
        if (itFind != mapObservers.end()) {
            if (std::ranges::find(itFind->second, observer) == itFind->second.end()) {
                itFind->second.push_back(observer);
            }
        } else {
            std::vector<Observer*> observers = {observer};
            mapObservers.try_emplace(notificationType, observers);
        }
    }

    void Observable::removeObserver(Observer* observer, int notificationType) const {
        auto itFind = mapObservers.find(notificationType);
        if (itFind != mapObservers.end()) {
            std::erase(itFind->second, observer);
        }
    }

    /**
    * @param observable Observable producing this notification
    */
    void Observable::notifyObservers(Observable* observable, int notificationType) {
        auto itFind = mapObservers.find(notificationType);
        if (itFind != mapObservers.end()) {
            for (Observer* observer : itFind->second) {
                observer->notify(observable, notificationType);
            }
        }
    }
}
