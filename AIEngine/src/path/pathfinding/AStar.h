#ifndef URCHINENGINE_ASTART_H
#define URCHINENGINE_ASTART_H

#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "path/navmesh/model/NavMesh.h"
#include "path/navmesh/model/NavTriangle.h"

namespace urchin
{

    struct TriangleRef
    {
        TriangleRef(unsigned int, unsigned int);

        unsigned int polygonIndex;
        unsigned int triangleIndex;
    };

    struct PathNode
    {
        PathNode(TriangleRef triangleRef, unsigned int, unsigned int);

        TriangleRef triangleRef;
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
            TriangleRef findTriangle(const Vector3<float> &) const;
            bool isPointInsideTriangle(const Point2<float> &, const std::shared_ptr<NavPolygon> &, const NavTriangle &) const;
            float sign(const Point2<float> &, const Point2<float> &, const Point2<float> &) const;

            float computeGScore(const TriangleRef &, const TriangleRef &) const;
            float computeHScore(const TriangleRef &, const TriangleRef &) const;
            std::shared_ptr<PathNode> retrievePathNode(const std::set<std::shared_ptr<PathNode>, PathNodeCompare> &, const TriangleRef &) const;

            std::shared_ptr<NavMesh> navMesh;
    };

}

#endif
