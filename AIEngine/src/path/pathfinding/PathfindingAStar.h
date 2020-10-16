#ifndef URCHINENGINE_ASTART_H
#define URCHINENGINE_ASTART_H

#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "path/navmesh/model/output/NavMesh.h"
#include "path/navmesh/model/output/NavTriangle.h"
#include "path/pathfinding/PathNode.h"
#include "path/pathfinding/PathPortal.h"
#include "path/PathPoint.h"

namespace urchin {

    struct PathNodeCompare {
        bool operator()(const std::shared_ptr<PathNode> &, const std::shared_ptr<PathNode> &) const;
    };

    class PathfindingAStar {
        public:
            explicit PathfindingAStar(std::shared_ptr<NavMesh>);

            std::vector<PathPoint> findPath(const Point3<float> &, const Point3<float> &) const;

        private:
            std::shared_ptr<NavTriangle> findTriangle(const Point3<float> &) const;
            bool isPointInsideTriangle(const Point2<float> &, const std::shared_ptr<NavPolygon> &, const std::shared_ptr<NavTriangle> &) const;
            float crossProduct(const Point2<float> &, const Point2<float> &, const Point2<float> &) const;

            std::shared_ptr<PathNode> retrievePathNodeFrom(const std::multiset<std::shared_ptr<PathNode>, PathNodeCompare> &, const std::shared_ptr<NavTriangle> &) const;
            float computeGScore(const std::shared_ptr<PathNode> &, const std::shared_ptr<NavLink> &, const Point3<float> &) const;
            float computeHScore(const std::shared_ptr<NavTriangle> &, const Point3<float> &) const;

            std::vector<std::shared_ptr<PathPortal>> determinePath(const std::shared_ptr<PathNode> &, const Point3<float> &, const Point3<float> &) const;
            LineSegment3D<float> rearrangePortal(const LineSegment3D<float> &, const std::vector<std::shared_ptr<PathPortal>> &) const;
            Point3<float> middlePoint(const LineSegment3D<float> &) const;

            std::vector<PathPoint> pathPortalsToPathPoints(std::vector<std::shared_ptr<PathPortal>> &, bool) const;
            void addMissingTransitionPoints(std::vector<std::shared_ptr<PathPortal>> &) const;
            Point3<float> computeTransitionPoint(const std::shared_ptr<PathPortal> &, const Point3<float> &) const;

            const float jumpAdditionalCost;
            std::shared_ptr<NavMesh> navMesh;
    };

}

#endif
