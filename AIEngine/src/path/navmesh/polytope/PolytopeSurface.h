#ifndef URCHINENGINE_POLYTOPESURFACE_H
#define URCHINENGINE_POLYTOPESURFACE_H

#include "UrchinCommon.h"

#include "path/navmesh/model/NavMeshAgent.h"
#include "path/navmesh/csg/CSGPolygon.h"

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
            virtual Rectangle<float> computeXZRectangle() const = 0;

            virtual std::vector<Point2<float>> getOutlineCwPoints() const = 0;
            virtual Plane<float> getPlane(const Rectangle<float> &, const NavMeshAgent &) const = 0;
            virtual std::vector<CSGPolygon<float>> getSelfObstacles() const = 0;
            virtual Point3<float> computeRealPoint(const Point2<float> &, const NavMeshAgent &) const = 0;

        private:
            bool walkableCandidate;
    };

}

#endif
