#pragma once

#include <vector>
#include <UrchinCommon.h>

#include "path/navmesh/model/output/NavTriangle.h"
#include "path/navmesh/model/output/NavPolygonEdge.h"
#include "path/navmesh/model/output/topography/NavTopography.h"

namespace urchin {

    class NavPolygon {
        public:
            NavPolygon(std::string, std::vector<Point3<float>>&&, std::shared_ptr<const NavTopography>);
            NavPolygon(const NavPolygon&);
            NavPolygon& operator=(const NavPolygon&) = delete;
            ~NavPolygon() = default;

            const std::string& getName() const;

            const std::vector<Point3<float>>& getPoints() const;
            const Point3<float>& getPoint(std::size_t) const;

            void addTriangles(const std::vector<std::shared_ptr<NavTriangle>>&, const std::shared_ptr<NavPolygon>&);
            const std::vector<std::shared_ptr<NavTriangle>>& getTriangles() const;

            const NavTopography* getNavTopography() const;
            const std::vector<NavPolygonEdge>& retrieveExternalEdges() const;

            void removeLinksTo(const NavPolygon&) const;

        private:
            std::string name;

            std::vector<Point3<float>> points;
            std::vector<std::shared_ptr<NavTriangle>> triangles;

            std::shared_ptr<const NavTopography> navTopography;
            mutable std::vector<NavPolygonEdge> externalEdges;
    };

}
