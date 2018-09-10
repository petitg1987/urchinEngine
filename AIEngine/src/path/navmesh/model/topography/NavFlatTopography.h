#ifndef URCHINENGINE_NAVFLATTOPOGRAPHY_H
#define URCHINENGINE_NAVFLATTOPOGRAPHY_H

#include "path/navmesh/model/topography/NavTopography.h"

namespace urchin
{

    class NavFlatTopography : public NavTopography
    {
        public:
            std::vector<Point3<float>> followTopography(const Point3<float> &, const Point3<float> &) const override;
    };

}

#endif
