#ifndef URCHINENGINE_ASTART_H
#define URCHINENGINE_ASTART_H

#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "path/navmesh/model/NavMesh.h"
#include "path/navmesh/model/NavTriangle.h"
#include "path/navmesh/model/NavTriangleRef.h"

namespace urchin
{

    struct PathNode
    {
        PathNode(NavTriangleRef, unsigned int, unsigned int);

        NavTriangleRef triangleRef;
        unsigned int gScore;
        unsigned int hScore;
        std::shared_ptr<PathNode> previous;
    };

    struct PathNodeCompare
    {
        bool operator()(const std::shared_ptr<PathNode> &, const std::shared_ptr<PathNode> &) const;
    };

    class AStar
    {
        public:
            explicit AStar(std::shared_ptr<NavMesh>);

            std::vector<NavTriangle> findPath(const Vector3<float> &, const Vector3<float> &) const;

        private:
            NavTriangleRef findTriangle(const Vector3<float> &) const;
            bool isPointInsideTriangle(const Point2<float> &, const std::shared_ptr<NavPolygon> &, const NavTriangle &) const;
            float sign(const Point2<float> &, const Point2<float> &, const Point2<float> &) const;

            uint_fast64_t computeTriangleId(const NavTriangleRef &) const;
            float computeGScore(std::shared_ptr<PathNode> &, const NavTriangleRef &) const;
            float computeHScore(const NavTriangleRef &, const NavTriangleRef &) const;
            std::shared_ptr<PathNode> retrievePathNodeFrom(const std::set<std::shared_ptr<PathNode>, PathNodeCompare> &, const NavTriangleRef &) const;

            std::shared_ptr<NavMesh> navMesh;
    };

}

#endif
