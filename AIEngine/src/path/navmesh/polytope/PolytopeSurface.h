#ifndef URCHINENGINE_POLYTOPESURFACE_H
#define URCHINENGINE_POLYTOPESURFACE_H

#include "UrchinCommon.h"

#include "path/navmesh/polytope/PolytopePoint.h"

namespace urchin
{

    class PolytopeSurface
    {
        public:
            PolytopeSurface();
            virtual ~PolytopeSurface() = default;

            void setWalkableCandidate(bool);
            bool isWalkableCandidate() const;

            virtual bool isWalkable(float maxSlopeInRadian) const = 0;
            virtual std::vector<Point2<float>> getOutlineCwPoints() const = 0;

        private:
            bool walkableCandidate;
    };

}

#endif
