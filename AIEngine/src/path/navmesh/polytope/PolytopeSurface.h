#pragma once

#include <UrchinCommon.h>

#include <path/navmesh/model/output/NavMeshAgent.h>
#include <path/navmesh/model/output/topography/NavTopography.h>
#include <path/navmesh/csg/CSGPolygon.h>

namespace urchin {

    class Polytope;

    class PolytopeSurface {
        public:
            PolytopeSurface();
            virtual ~PolytopeSurface() = default;

            void setPolytope(const Polytope*);
            const Polytope* getPolytope() const;
            std::size_t getSurfacePosition() const;

            void setWalkableCandidate(bool);
            bool isWalkableCandidate() const;

            virtual bool isWalkable() const = 0;
            virtual Rectangle<float> computeXZRectangle() const = 0;
            virtual const AABBox<float>& getAABBox() const = 0;

            virtual const std::vector<Point2<float>>& getOutlineCwPoints() const = 0;
            virtual Plane<float> getPlane(const Rectangle<float>&) const = 0;
            virtual const std::vector<CSGPolygon<float>>& getSelfObstacles() const = 0;
            virtual Point3<float> computeRealPoint(const Point2<float>&, const std::shared_ptr<NavMeshAgent>&) const = 0;
            virtual const std::shared_ptr<const NavTopography>& getNavTopography() const = 0;

        private:
            std::size_t computeSurfacePosition();

            const Polytope* polytope;
            std::size_t surfacePosition; //position/index of surface in polytope
            bool walkableCandidate;
    };

}
