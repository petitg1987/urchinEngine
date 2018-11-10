#ifndef URCHINENGINE_PATHPORTAL_H
#define URCHINENGINE_PATHPORTAL_H

#include "UrchinCommon.h"
#include "path/pathfinding/PathNode.h"

namespace urchin
{

    class PathPortal
    {
        public:
            PathPortal(const LineSegment3D<float> &, const std::shared_ptr<PathNode> &, const std::shared_ptr<PathNode> &);

            void setPivotPoint(const Point3<float> &);
            bool hasPivotPoint() const;
            const Point3<float> &getPivotPoint() const;

            const LineSegment3D<float> &getPortal() const;
            const std::shared_ptr<PathNode> &getPreviousPathNode() const;
            const std::shared_ptr<PathNode> &getNextPathNode() const;

        private:
            LineSegment3D<float> portal;
            std::shared_ptr<PathNode> previousPathNode;
            std::shared_ptr<PathNode> nextPathNode;

            Point3<float> pivotPoint;
            bool bHasPivotPoint;
    };

}

#endif
