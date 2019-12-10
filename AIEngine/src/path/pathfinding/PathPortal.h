#ifndef URCHINENGINE_PATHPORTAL_H
#define URCHINENGINE_PATHPORTAL_H

#include "UrchinCommon.h"
#include "path/pathfinding/PathNode.h"

namespace urchin
{

    class PathPortal
    {
        public:
            PathPortal(LineSegment3D<float>, std::shared_ptr<PathNode>, std::shared_ptr<PathNode>, bool);

            void setPivotPoint(const Point3<float> &);
            bool hasPivotPoint() const;
            const Point3<float> &getPivotPoint() const;

            bool isJumpPortal() const;

            const LineSegment3D<float> &getPortal() const;
            const std::shared_ptr<PathNode> &getPreviousPathNode() const;
            const std::shared_ptr<PathNode> &getNextPathNode() const;

        private:
            LineSegment3D<float> portal;
            std::shared_ptr<PathNode> previousPathNode;
            std::shared_ptr<PathNode> nextPathNode;
            bool bIsJumpPortal;

            Point3<float> pivotPoint;
            bool bHasPivotPoint; //TODO review because pivotPoint is also used for jump (not very logical)
    };

}

#endif
