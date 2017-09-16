#include <algorithm>

#include "PathfindingAStar.h"

namespace urchin
{

    bool PathNodeCompare::operator()(const std::shared_ptr<PathNode> &node1, const std::shared_ptr<PathNode> &node2) const
    {
        return node1->getFScore() < node2->getFScore();
    }

    PathfindingAStar::PathfindingAStar(std::shared_ptr<NavMesh> navMesh) :
            navMesh(std::move(navMesh))
    {

    }

    std::vector<Point3<float>> PathfindingAStar::findPath(const Point3<float> &startPoint, const Point3<float> &endPoint) const
    {
        NavTriangleRef startTriangle = findTriangle(startPoint);
        NavTriangleRef endTriangle = findTriangle(endPoint);

        float startEndHScore = computeHScore(startTriangle, endPoint);

        std::set<uint_fast64_t> closedList;
        std::multiset<std::shared_ptr<PathNode>, PathNodeCompare> openList;
        openList.insert(std::make_shared<PathNode>(startTriangle, 0.0, startEndHScore));

        while(!openList.empty())
        {
            auto currentNodeIt = openList.begin(); //node with smallest fScore
            std::shared_ptr<PathNode> currentNode = *currentNodeIt;

            closedList.insert(computeTriangleId(currentNode->getTriangleRef()));
            openList.erase(currentNodeIt);

            if(currentNode->getTriangleRef().equals(endTriangle))
            { //path is found
                return determinePath(currentNode, startPoint, endPoint);
            }

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
                        float gScore = computeGScore(currentNode, neighborTriangleRef);
                        float hScore = computeHScore(neighborTriangleRef, endPoint);
                        neighborNodePath.reset(new PathNode(neighborTriangleRef, gScore, hScore));
                        linkPathNodes(currentNode, neighborNodePath, i);
                        openList.insert(neighborNodePath);
                    }else
                    {
                        float gScore = computeGScore(currentNode, neighborTriangleRef);
                        if(neighborNodePath->getGScore() > gScore)
                        { //better path found to reach neighborNodePath: override previous values
                            neighborNodePath->setGScore(gScore);
                            linkPathNodes(currentNode, neighborNodePath, i);
                        }
                    }
                }
            }

            if (openList.empty())
            { //no path exists
                return {};
            }
        }
    }

    NavTriangleRef PathfindingAStar::findTriangle(const Point3<float> &point) const
    {
        for(unsigned int polyIndex=0; polyIndex<navMesh->getPolygons().size(); ++polyIndex)
        {
            std::shared_ptr<NavPolygon> polygon = navMesh->getPolygons()[polyIndex];
            //TODO check if point inside 'polygon' (BBox or Sphere)
            for(unsigned int triIndex=0; triIndex<polygon->getTriangles().size(); ++triIndex)
            {
                const NavTriangle &triangle = polygon->getTriangles()[triIndex];
                Point2<float> flattenPoint(point.X, point.Z);

                if(isPointInsideTriangle(flattenPoint, polygon, triangle))
                { //TODO check the Y axis (point nearest to triangle if several triangles match)
                    return {polyIndex, triIndex};
                }
            }
        }
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

    float PathfindingAStar::computeGScore(std::shared_ptr<PathNode> &currentNode, const NavTriangleRef &neighbor) const
    { //TODO use funnel algorithm ?
        Point3<float> currentPoint = navMesh->resolveTriangle(currentNode->getTriangleRef()).getCenterPoint();
        Point3<float> neighborPoint = navMesh->resolveTriangle(neighbor).getCenterPoint();

        float gScoreCurrentToNeighbor = currentPoint.distance(neighborPoint);
        return currentNode->getGScore() + gScoreCurrentToNeighbor;
    }

    float PathfindingAStar::computeHScore(const NavTriangleRef &current, const Point3<float> &endPoint) const
    {
        Point3<float> currentPoint = navMesh->resolveTriangle(current).getCenterPoint();
        return std::abs(currentPoint.X - endPoint.X) + std::abs(currentPoint.Y - endPoint.Y) + std::abs(currentPoint.Z - endPoint.Z);
    }

    void PathfindingAStar::linkPathNodes(const std::shared_ptr<PathNode> &currentNode, const std::shared_ptr<PathNode> &nextNode,
                                         unsigned int currentNodeLinkEdgeId) const
    {
        nextNode->setPrevious(currentNode);
        currentNode->setEdgeId(currentNodeLinkEdgeId);
    }

    std::vector<Point3<float>> PathfindingAStar::determinePath(const std::shared_ptr<PathNode> &endNode, const Point3<float> &startPoint,
                                                               const Point3<float> &endPoint) const
    {
        std::vector<LineSegment3D<float>> portals;
        portals.reserve(10); //estimated memory size

        std::shared_ptr<PathNode> pathNode = endNode->getPrevious();
        while(pathNode!=nullptr)
        {
            portals.push_back(navMesh->resolveEdge(pathNode->retrieveNavEdgeRef()));
            pathNode = pathNode->getPrevious();
        }
        std::reverse(portals.begin(), portals.end());

        if(portals.empty())
        {
            return {startPoint, endPoint};
        }else
        {
            //TODO use funnel algorithm
        }

        return {};
    }
}
