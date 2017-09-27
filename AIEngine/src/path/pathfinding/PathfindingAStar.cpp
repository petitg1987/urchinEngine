#include <algorithm>

#include "PathfindingAStar.h"
#include "path/pathfinding/FunnelAlgorithm.h"

namespace urchin
{

    bool PathNodeCompare::operator()(const std::shared_ptr<PathNode> &node1, const std::shared_ptr<PathNode> &node2) const
    {
        return node1->getFScore() < node2->getFScore();
    }

    PathfindingAStar::PathfindingAStar(std::shared_ptr<NavMesh> navMesh) :
            navMesh(std::move(navMesh))
    {
        #ifdef _DEBUG
//            this->navMesh->logNavMesh();
        #endif
    }

    std::vector<Point3<float>> PathfindingAStar::findPath(const Point3<float> &startPoint, const Point3<float> &endPoint) const
    {
        std::unique_ptr<NavTriangleRef> startTriangle = findTriangle(startPoint);
        std::unique_ptr<NavTriangleRef>  endTriangle = findTriangle(endPoint);
        if(startTriangle==nullptr || endTriangle==nullptr)
        {
            return {}; //no path exists
        }

        float startEndHScore = computeHScore(*startTriangle, endPoint);

        std::set<uint_fast64_t> closedList;
        std::multiset<std::shared_ptr<PathNode>, PathNodeCompare> openList;
        openList.insert(std::make_shared<PathNode>(*startTriangle, 0.0, startEndHScore));

        std::shared_ptr<PathNode> endNodePath = nullptr;
        while(!openList.empty())
        {
            auto currentNodeIt = openList.begin(); //node with smallest fScore
            std::shared_ptr<PathNode> currentNode = *currentNodeIt;

            closedList.insert(computeTriangleId(currentNode->getTriangleRef()));
            openList.erase(currentNodeIt);

            const NavTriangle &currTriangle = navMesh->resolveTriangle(currentNode->getTriangleRef());
            for(unsigned int i=0; i<3; ++i)
            {
                if(currTriangle.getNeighbor(i)!=-1)
                {
                    auto neighborTriangleId = static_cast<unsigned int>(currTriangle.getNeighbor(i));
                    NavTriangleRef neighborTriangleRef(currentNode->getTriangleRef().getPolygonIndex(), neighborTriangleId);

                    if(closedList.find(computeTriangleId(neighborTriangleRef))!=closedList.end())
                    { //already processed
                        continue;
                    }

                    std::shared_ptr<PathNode> neighborNodePath = retrievePathNodeFrom(openList, neighborTriangleRef);
                    if(!neighborNodePath)
                    {
                        float gScore = computeGScore(currentNode, neighborTriangleRef, startPoint, endPoint, i);
                        float hScore = computeHScore(neighborTriangleRef, endPoint);
                        neighborNodePath.reset(new PathNode(neighborTriangleRef, gScore, hScore));
                        neighborNodePath->setPreviousNode(currentNode, i);

                        if(!endNodePath || neighborNodePath->getFScore() < endNodePath->getFScore())
                        {
                            openList.insert(neighborNodePath);
                        }

                        if(neighborTriangleRef.equals(*endTriangle))
                        { //end triangle reached but continue on path nodes having a smaller F score
                            endNodePath = neighborNodePath;
                        }
                    }else
                    {
                        float gScore = computeGScore(currentNode, neighborTriangleRef, startPoint, endPoint, i);
                        if(neighborNodePath->getGScore() > gScore)
                        { //better path found to reach neighborNodePath: override previous values
                            neighborNodePath->setGScore(gScore);
                            neighborNodePath->setPreviousNode(currentNode, i);
                        }
                    }
                }
            }
        }

        if(endNodePath)
        {
            return determinePath(endNodePath, startPoint, endPoint);
        }

        return {}; //no path exists
    }

    std::unique_ptr<NavTriangleRef> PathfindingAStar::findTriangle(const Point3<float> &point) const
    {
        float bestVerticalDistance = std::numeric_limits<float>::max();
        std::unique_ptr<NavTriangleRef> triangleRef = nullptr;

        for(unsigned int polyIndex=0; polyIndex<navMesh->getPolygons().size(); ++polyIndex)
        {
            std::shared_ptr<NavPolygon> polygon = navMesh->getPolygons()[polyIndex];
            for(unsigned int triIndex=0; triIndex<polygon->getTriangles().size(); ++triIndex)
            {
                const NavTriangle &triangle = polygon->getTriangles()[triIndex];
                Point2<float> flattenPoint(point.X, point.Z);

                if(isPointInsideTriangle(flattenPoint, polygon, triangle))
                {
                    float verticalDistance = point.Y - triangle.getCenterPoint().Y;
                    if(verticalDistance > 0.0 && verticalDistance < bestVerticalDistance)
                    {
                        bestVerticalDistance = verticalDistance;
                        triangleRef = std::make_unique<NavTriangleRef>(polyIndex, triIndex);
                    }
                }
            }
        }
        return triangleRef;
    }

    bool PathfindingAStar::isPointInsideTriangle(const Point2<float> &point, const std::shared_ptr<NavPolygon> &polygon, const NavTriangle &triangle) const
    {
        Point3<float> p0 = polygon->getPoint(triangle.getIndex(0));
        Point3<float> p1 = polygon->getPoint(triangle.getIndex(1));
        Point3<float> p2 = polygon->getPoint(triangle.getIndex(2));

        bool b1 = sign(point, Point2<float>(p0.X, p0.Z), Point2<float>(p1.X, p1.Z)) < 0.0f;
        bool b2 = sign(point, Point2<float>(p1.X, p1.Z), Point2<float>(p2.X, p2.Z)) < 0.0f;
        bool b3 = sign(point, Point2<float>(p2.X, p2.Z), Point2<float>(p0.X, p0.Z)) < 0.0f;

        return ((b1 == b2) && (b2 == b3));
    }

    float PathfindingAStar::sign(const Point2<float> &p1, const Point2<float> &p2, const Point2<float> &p3) const
    {
        return (p1.X - p3.X) * (p2.Y - p3.Y) - (p2.X - p3.X) * (p1.Y - p3.Y);
    }

    uint_fast64_t PathfindingAStar::computeTriangleId(const NavTriangleRef &triangle) const
    {
        auto id = static_cast<uint_fast64_t>(triangle.getPolygonIndex());
        id = id << 32;
        return id + triangle.getTriangleIndex();
    }

    std::shared_ptr<PathNode> PathfindingAStar::retrievePathNodeFrom(const std::multiset<std::shared_ptr<PathNode>, PathNodeCompare> &pathNodes,
                                                                     const NavTriangleRef &triangleRef) const
    {
        for(const auto &pathNode : pathNodes)
        {
            if(pathNode->getTriangleRef().equals(triangleRef))
            {
                return pathNode;
            }
        }
        return std::shared_ptr<PathNode>();
    }

    float PathfindingAStar::computeGScore(std::shared_ptr<PathNode> &currentNode, const NavTriangleRef &neighbor, const Point3<float> &startPoint,
                                          const Point3<float> &endPoint, unsigned previousNodeLinkEdgeId) const
    {
        Point3<float> neighborPoint = navMesh->resolveTriangle(neighbor).getCenterPoint();

        std::shared_ptr<PathNode> neighborNodePath = std::make_shared<PathNode>(neighbor, 0.0, 0.0);
        neighborNodePath->setPreviousNode(currentNode, previousNodeLinkEdgeId);
        std::vector<Point3<float>> path = determinePath(neighborNodePath, startPoint, neighborPoint);

        float pathDistance = 0.0;
        for(unsigned int i=0; i<path.size()-1; i++)
        {
            pathDistance += path[i].distance(path[i+1]);
        }

        return pathDistance;
    }

    float PathfindingAStar::computeHScore(const NavTriangleRef &current, const Point3<float> &endPoint) const
    {
        Point3<float> currentPoint = navMesh->resolveTriangle(current).getCenterPoint();
        return std::abs(currentPoint.X - endPoint.X) + std::abs(currentPoint.Y - endPoint.Y) + std::abs(currentPoint.Z - endPoint.Z);
    }

    std::vector<Point3<float>> PathfindingAStar::determinePath(const std::shared_ptr<PathNode> &endNode, const Point3<float> &startPoint,
                                                               const Point3<float> &endPoint) const
    {
        auto portals = std::make_shared<std::vector<LineSegment3D<float>>>();
        portals->reserve(10); //estimated memory size

        portals->emplace_back(LineSegment3D<float>(endPoint, endPoint));
        std::shared_ptr<PathNode> pathNode = endNode;
        while(pathNode->getPreviousNode()!=nullptr)
        {
            LineSegment3D<float> portal = navMesh->resolveEdge(pathNode->retrieveNavEdgeRef());

            Point3<float> characterPosition = middlePoint((*portals)[portals->size()-1]);
            Vector3<float> characterMoveDirection = characterPosition.vector(middlePoint(portal)).normalize();
            Vector3<float> characterToPortalA = characterPosition.vector(portal.getA()).normalize();
            float crossProductY = characterMoveDirection.Z*characterToPortalA.X - characterMoveDirection.X*characterToPortalA.Z;
            if(crossProductY <= 0.0)
            {
                portals->emplace_back(portal);
            }else
            {
                portals->emplace_back(LineSegment3D<float>(portal.getB(), portal.getA()));
            }

            pathNode = pathNode->getPreviousNode();
        }
        portals->emplace_back(LineSegment3D<float>(startPoint, startPoint));
        std::reverse(portals->begin(), portals->end());

        FunnelAlgorithm funnelAlgorithm(portals);
        return funnelAlgorithm.findPath();
    }

    Point3<float> PathfindingAStar::middlePoint(const LineSegment3D<float> &lineSegment) const
    {
        return (lineSegment.getA() + lineSegment.getB()) / 2.0f;
    }
}
