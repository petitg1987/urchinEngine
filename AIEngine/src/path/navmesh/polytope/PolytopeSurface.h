#ifndef URCHINENGINE_POLYTOPESURFACE_H
#define URCHINENGINE_POLYTOPESURFACE_H

#include "UrchinCommon.h"

#include "path/navmesh/polytope/PolytopePoint.h"
#include "path/navmesh/model/NavMeshAgent.h"

namespace urchin
{

    class PolytopeSurface
    {
        public:
            PolytopeSurface(bool);
            virtual ~PolytopeSurface() = default;

            bool isExpandedSurface() const;

            void setWalkableCandidate(bool);
            bool isWalkableCandidate() const;

            virtual bool isWalkable(float maxSlopeInRadian) const = 0;
            virtual Rectangle<float> computeXZRectangle() const = 0;

            virtual std::vector<Point2<float>> getOutlineCwPoints() const = 0;
            virtual Plane<float> getExpandedPlane(const Rectangle<float> &, const NavMeshAgent &) const = 0;
            virtual Point3<float> elevatePoint(const Point2<float> &, const NavMeshAgent &) const = 0;

        private:
            bool expandedSurface;
            bool walkableCandidate;
    };

}

#endif
