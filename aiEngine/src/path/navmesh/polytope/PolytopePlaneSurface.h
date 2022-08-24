#pragma once

#include <vector>
#include <initializer_list>
#include <UrchinCommon.h>

#include <path/navmesh/polytope/PolytopeSurface.h>

namespace urchin {

    class PolytopePlaneSurface final : public PolytopeSurface {
        public:
            PolytopePlaneSurface(std::vector<Point3<float>>, bool);
            PolytopePlaneSurface(std::vector<Point3<float>>, const Vector3<float>&, bool);

            bool isWalkable() const override;
            Rectangle2D<float> computeXZRectangle() const override;
            const AABBox<float>& getAABBox() const override;

            const std::vector<Point2<float>>& getOutlineCwPoints() const override;
            Plane<float> getPlane(const Rectangle2D<float>&) const override;
            const std::vector<CSGPolygon<float>>& getSelfObstacles() const override;
            Point3<float> computeRealPoint(const Point2<float>&, const NavMeshAgent&) const override;
            const std::shared_ptr<const NavTopography>& getNavTopography() const override;

            const std::vector<Point3<float>>& getCcwPoints() const;
            const Vector3<float>& getNormal() const;

        private:
            void buildOutlineCwPoints();
            void buildAABBox();

            std::vector<Point3<float>> ccwPoints;
            Vector3<float> normal;
            bool isSlopeWalkable;

            std::vector<Point2<float>> outlineCwPoints;
            std::vector<CSGPolygon<float>> selfObstacles;

            std::shared_ptr<const NavTopography> nullNavTopography;

            AABBox<float> aabbox;
    };

}
