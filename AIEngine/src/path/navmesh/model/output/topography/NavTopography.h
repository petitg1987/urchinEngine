#ifndef URCHINENGINE_NAVTOPOGRAPHY_H
#define URCHINENGINE_NAVTOPOGRAPHY_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

     class NavTopography
    {
        public:
            virtual ~NavTopography() = default;

            virtual std::vector<Point3<float>> followTopography(const Point3<float> &, const Point3<float> &) const = 0;

    };

}

#endif
