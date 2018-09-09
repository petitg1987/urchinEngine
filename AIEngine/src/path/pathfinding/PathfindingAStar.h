#ifndef URCHINENGINE_ASTART_H
#define URCHINENGINE_ASTART_H

#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "path/navmesh/model/NavMesh.h"
#include "path/navmesh/model/NavTriangle.h"
#include "path/pathfinding/PathNode.h"

namespace urchin
{

    struct PathNodeCompare
    {
        bool operator()(const std::shared_ptr<PathNode> &, const std::shared_ptr<PathNode> &) const;
    };

    class PathfindingAStar
    {
        public:
            explicit PathfindingAStar(std::shared_ptr<NavMesh>);

            std::vector<Point3<float>> findPath(const Point3<float> &, const Point3<float> &) const;

        private:
            std::shared_ptr<NavTriangle> findTriangle(const Point3<float> &) const;
            bool isPointInsideTriangle(const Point2<float> &, const std::shared_ptr<NavPolygon> &, const std::shared_ptr<NavTriangle> &) const;
            float sign(const Point2<float> &, const Point2<float> &, const Point2<float> &) const;

            std::shared_ptr<PathNode> retrievePathNodeFrom(const std::multiset<std::shared_ptr<PathNode>, PathNodeCompare> &, const std::shared_ptr<NavTriangle> &) const;
            float computeGScore(std::shared_ptr<PathNode> &, const std::shared_ptr<NavTriangle> &, const Point3<float> &, const Point3<float> &, unsigned int) const;
            float computeHScore(const std::shared_ptr<NavTriangle> &, const Point3<float> &) const;

            std::vector<Point3<float>> determinePath(const std::shared_ptr<PathNode> &, const Point3<float> &, const Point3<float> &) const;
            Point3<float> middlePoint(const LineSegment3D<float> &) const;

            std::shared_ptr<NavMesh> navMesh;
    };

}

#endif
