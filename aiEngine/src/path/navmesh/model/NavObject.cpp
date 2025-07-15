#include <path/navmesh/model/NavObject.h>

namespace urchin {

    NavObject::NavObject() {
        navPolygons.reserve(4); //estimated memory size
    }

    void NavObject::addNavPolygons(const std::vector<std::shared_ptr<NavPolygon>>& navPolygonsToAdd) {
        navPolygons.insert(navPolygons.end(), navPolygonsToAdd.begin(), navPolygonsToAdd.end());
    }

    const std::vector<std::shared_ptr<NavPolygon>>& NavObject::getNavPolygons() const {
        return navPolygons;
    }

    void NavObject::removeAllNavPolygons() {
        navPolygons.clear();
    }
}
