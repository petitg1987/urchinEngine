#ifndef URCHINENGINE_POLYGONSSUBTRACTION_H
#define URCHINENGINE_POLYGONSSUBTRACTION_H

#include <vector>
#include <map>
#include <string>
#include "UrchinCommon.h"

#include "CSGPolygon.h"

namespace urchin {

    template<class T> class PolygonsSubtraction : public Singleton<PolygonsSubtraction<T>> {
        public:
            friend class Singleton<PolygonsSubtraction<T>>;

            const std::vector<CSGPolygon<T>> &subtractPolygons(const CSGPolygon<T>&, const CSGPolygon<T>&) const;
            const std::vector<CSGPolygon<T>> &subtractPolygons(const CSGPolygon<T>&, const CSGPolygon<T>&, bool&) const;

        private:
            PolygonsSubtraction() = default;
            ~PolygonsSubtraction() override = default;

            mutable std::vector<CSGPolygon<T>> subtractedPolygons;
    };

}

#endif
