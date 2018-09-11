#include <algorithm>

#include "PathfindingAStar.h"
#include "path/pathfinding/PathPortal.h"
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
            //this->navMesh->logNavMesh();
        #endif
    }

    std::vector<Point3<float>> PathfindingAStar::findPath(const Point3<float> &startPoint, const Point3<float> &endPoint) const
    {
        std::shared_ptr<NavTriangle> startTriangle = findTriangle(startPoint);
        std::shared_ptr<NavTriangle> endTriangle = findTriangle(endPoint);
        if(startTriangle==nullptr || endTriangle==nullptr)
        {
            return {}; //no path exists
        }

        float startEndHScore = computeHScore(startTriangle, endPoint);

        std::set<std::shared_ptr<NavTriangle>> closedList;
        std::multiset<std::shared_ptr<PathNode>, PathNodeCompare> openList;
        openList.insert(std::make_shared<PathNode>(startTriangle, 0.0, startEndHScore));

        std::shared_ptr<PathNode> endNodePath = nullptr;
        while(!openList.empty())
        {
            auto currentNodeIt = openList.begin(); //node with smallest fScore
            std::shared_ptr<PathNode> currentNode = *currentNodeIt;

            closedList.insert(currentNode->getNavTriangle());
            openList.erase(currentNodeIt);

            const auto currTriangle = currentNode->getNavTriangle();
            for(unsigned int i=0; i<3; ++i)
            {
                if(currentNode->getNavTriangle()->getNeighbor(i) != nullptr)
                {
                    const auto &neighborTriangle = currentNode->getNavTriangle()->getNeighbor(i);

                    if(closedList.find(neighborTriangle)!=closedList.end())
                    { //already processed
                        continue;
                    }

                    std::shared_ptr<PathNode> neighborNodePath = retrievePathNodeFrom(openList, neighborTriangle);
                    if(!neighborNodePath)
                    {
                        float gScore = computeGScore(currentNode, neighborTriangle, startPoint, endPoint, i);
                        float hScore = computeHScore(neighborTriangle, endPoint);
                        neighborNodePath.reset(new PathNode(neighborTriangle, gScore, hScore));
                        neighborNodePath->setPreviousNode(currentNode, i);

                        if(!endNodePath || neighborNodePath->getFScore() < endNodePath->getFScore())
                        {
                            openList.insert(neighborNodePath);
                        }

                        if(neighborTriangle.get() == endTriangle.get())
                        { //end triangle reached but continue on path nodes having a smaller F score
                            endNodePath = neighborNodePath;
                        }
                    }else
                    {
                        float gScore = computeGScore(currentNode, neighborTriangle, startPoint, endPoint, i);
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

        #ifdef _DEBUG
//            navMesh->svgMeshExport(std::string(std::getenv("HOME")) + "/pathInfo/pathInfo" + std::to_string(navMesh->getId()) + ".html");
        #endif
        return {}; //no path exists
    }

    std::shared_ptr<NavTriangle> PathfindingAStar::findTriangle(const Point3<float> &point) const
    {
        float bestVerticalDistance = std::numeric_limits<float>::max();
        std::shared_ptr<NavTriangle> result = nullptr;

        for (const auto &polygon : navMesh->getPolygons())
        {
            for (unsigned int triIndex = 0; triIndex < polygon->getTriangles().size(); ++triIndex)
            {
                const auto &triangle = polygon->getTriangles()[triIndex];
                Point2<float> flattenPoint(point.X, point.Z);

                if (isPointInsideTriangle(flattenPoint, polygon, triangle))
                {
                    float verticalDistance = point.Y - triangle->getCenterPoint().Y;
                    if (verticalDistance > 0.0 && verticalDistance < bestVerticalDistance)
                    {
                        bestVerticalDistance = verticalDistance;
                        result = triangle;
                    }
                }
            }
        }
        return result;
    }

    bool PathfindingAStar::isPointInsideTriangle(const Point2<float> &point, const std::shared_ptr<NavPolygon> &polygon, const std::shared_ptr<NavTriangle> &triangle) const
    {
        Point3<float> p0 = polygon->getPoint(triangle->getIndex(0));
        Point3<float> p1 = polygon->getPoint(triangle->getIndex(1));
        Point3<float> p2 = polygon->getPoint(triangle->getIndex(2));

        bool b1 = sign(point, Point2<float>(p0.X, p0.Z), Point2<float>(p1.X, p1.Z)) < 0.0f;
        bool b2 = sign(point, Point2<float>(p1.X, p1.Z), Point2<float>(p2.X, p2.Z)) < 0.0f;
        bool b3 = sign(point, Point2<float>(p2.X, p2.Z), Point2<float>(p0.X, p0.Z)) < 0.0f;

        return ((b1 == b2) && (b2 == b3));
    }

    float PathfindingAStar::sign(const Point2<float> &p1, const Point2<float> &p2, const Point2<float> &p3) const
    {
        return (p1.X - p3.X) * (p2.Y - p3.Y) - (p2.X - p3.X) * (p1.Y - p3.Y);
    }

    std::shared_ptr<PathNode> PathfindingAStar::retrievePathNodeFrom(const std::multiset<std::shared_ptr<PathNode>, PathNodeCompare> &pathNodes,
                                                                     const std::shared_ptr<NavTriangle> &navTriangle) const
    {
        for(const auto &pathNode : pathNodes)
        {
            if(pathNode->getNavTriangle().get() == navTriangle.get())
            {
                return pathNode;
            }
        }
        return std::shared_ptr<PathNode>();
    }

    float PathfindingAStar::computeGScore(std::shared_ptr<PathNode> &currentNode, const std::shared_ptr<NavTriangle> &neighbor, const Point3<float> &startPoint,
                                          const Point3<float> &endPoint, unsigned previousNodeLinkEdgeId) const
    {
        std::shared_ptr<PathNode> neighborNodePath = std::make_shared<PathNode>(neighbor, 0.0, 0.0);
        neighborNodePath->setPreviousNode(currentNode, previousNodeLinkEdgeId);
        std::vector<Point3<float>> path = determinePath(neighborNodePath, startPoint, neighbor->getCenterPoint());

        float pathDistance = 0.0;
        for(unsigned int i=0; i<path.size()-1; i++)
        {
            pathDistance += path[i].distance(path[i+1]);
        }

        return pathDistance;
    }

    float PathfindingAStar::computeHScore(const std::shared_ptr<NavTriangle> &current, const Point3<float> &endPoint) const
    {
        Point3<float> currentPoint = current->getCenterPoint();
        return std::abs(currentPoint.X - endPoint.X) + std::abs(currentPoint.Y - endPoint.Y) + std::abs(currentPoint.Z - endPoint.Z);
    }

    std::vector<Point3<float>> PathfindingAStar::determinePath(const std::shared_ptr<PathNode> &endNode, const Point3<float> &startPoint,
                                                               const Point3<float> &endPoint) const
    {
        std::vector<std::shared_ptr<PathPortal>> portals;
        portals.reserve(10); //estimated memory size

        std::shared_ptr<PathNode> pathNode = endNode;
        std::shared_ptr<PathPortal> endPortal = std::make_shared<PathPortal>(LineSegment3D<float>(endPoint, endPoint), pathNode, nullptr);
        portals.emplace_back(endPortal);
        while(pathNode->getPreviousNode()!=nullptr)
        {
            LineSegment3D<float> portal = pathNode->computeEdgeWithPreviousNode();

            Point3<float> characterPosition = middlePoint(portals.back()->getPortal());
            Vector3<float> characterMoveDirection = characterPosition.vector(middlePoint(portal)).normalize();
            Vector3<float> characterToPortalA = characterPosition.vector(portal.getA()).normalize();
            float crossProductY = characterMoveDirection.Z*characterToPortalA.X - characterMoveDirection.X*characterToPortalA.Z;
            if(crossProductY > 0.0f)
            {
                portal = LineSegment3D<float>(portal.getB(), portal.getA());
            }

            portals.emplace_back(std::make_shared<PathPortal>(portal, pathNode->getPreviousNode(), pathNode));

            pathNode = pathNode->getPreviousNode();
        }
        portals.emplace_back(std::make_shared<PathPortal>(LineSegment3D<float>(startPoint, startPoint), nullptr, pathNode));
        std::reverse(portals.begin(), portals.end());

        FunnelAlgorithm funnelAlgorithm(portals);
        return funnelAlgorithm.findPath();
    }

    Point3<float> PathfindingAStar::middlePoint(const LineSegment3D<float> &lineSegment) const
    {
        return (lineSegment.getA() + lineSegment.getB()) / 2.0f;
    }
}
