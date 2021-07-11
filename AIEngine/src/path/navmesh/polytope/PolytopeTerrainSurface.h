#pragma once

#include <memory>

#include <UrchinCommon.h>
#include <path/navmesh/polytope/PolytopeSurface.h>

namespace urchin {

    class PolytopeTerrainSurface : public PolytopeSurface {
        public:
            PolytopeTerrainSurface(const Point3<float>&, std::vector<Point3<float>>, unsigned int, unsigned int,
                    const Vector3<float>&, std::vector<CSGPolygon<float>>, std::shared_ptr<const NavTopography>);

            bool isWalkable() const override;
            Rectangle<float> computeXZRectangle() const override;
            const AABBox<float>& getAABBox() const override;

            const std::vector<Point2<float>>& getOutlineCwPoints() const override;
            Plane<float> getPlane(const Rectangle<float>&) const override;
            const std::vector<CSGPolygon<float>>& getSelfObstacles() const override;
            Point3<float> computeRealPoint(const Point2<float>&, const std::shared_ptr<NavMeshAgent>&) const override;
            const std::shared_ptr<const NavTopography>& getNavTopography() const override;

            const Point3<float>& getPosition() const;
            const std::vector<Point3<float>>& getLocalVertices() const;
            unsigned int getXLength() const;
            unsigned int getZLength() const;
            const Vector3<float>& getApproximateNormal() const;

        private:
            void buildOutlineCwPoints();
            void buildAABBox();
            Point3<float> retrieveGlobalVertex(const Point2<float>&) const;

            Point3<float> position;
            std::vector<Point3<float>> localVertices;
            unsigned int xLength;
            unsigned int zLength;
            Vector3<float> approximateNormal;

            std::vector<Point2<float>> outlineCwPoints;
            std::vector<CSGPolygon<float>> selfObstacles;

            std::unique_ptr<HeightfieldPointHelper<float>> heightfieldPointHelper;
            std::shared_ptr<const NavTopography> navTopography;

            AABBox<float> aabbox;
    };

}
