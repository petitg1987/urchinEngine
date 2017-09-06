#include "AStar.h"

namespace urchin
{
    TriangleRef::TriangleRef(unsigned int polygonIndex, unsigned int triangleIndex) :
            polygonIndex(polygonIndex),
            triangleIndex(triangleIndex)
    {

    }

    PathNode::PathNode(TriangleRef triangleRef, unsigned int gScore, unsigned int hScore) :
            triangleRef(triangleRef),
            gScore(gScore),
            hScore(hScore)
    {

    }

    bool PathNodeCompare::operator()(const std::shared_ptr<PathNode> &node1, const std::shared_ptr<PathNode> &node2) const
    {
        return node1->gScore+node1->hScore < node2->gScore+node2->hScore;
    }

    AStar::AStar(std::shared_ptr<NavMesh> navMesh) :
            navMesh(std::move(navMesh))
    {

    }

    std::vector<NavTriangle> AStar::findPath(const Vector3<float> &start, const Vector3<float> &end) const
    {
        TriangleRef startTriangle = findTriangle(start);
        TriangleRef endTriangle = findTriangle(end);
        float startEndHScore = computeHScore(startTriangle, endTriangle);

        std::set<std::shared_ptr<PathNode>> closedList;
        std::set<std::shared_ptr<PathNode>, PathNodeCompare> openList;
        openList.insert(std::make_shared<PathNode>(startTriangle, 0.0, startEndHScore));

        while(!openList.empty())
        {
            std::shared_ptr<PathNode> currentNode = *openList.begin(); //node with smallest fScore

            closedList.insert(currentNode);
            openList.erase(currentNode);

            if(currentNode->triangleRef.polygonIndex==endTriangle.polygonIndex && currentNode->triangleRef.triangleIndex==endTriangle.triangleIndex)
            { //path is found
                //TODO return currentNode->previous->previous
            }

            const NavTriangle &currTriangle = navMesh->getPolygons()[currentNode->triangleRef.polygonIndex]->getTriangle(currentNode->triangleRef.triangleIndex);
            for(unsigned int i=0; i<3; ++i)
            {
                if(currTriangle.getNeighbor(i)!=-1)
                {
                    TriangleRef neighborTriangleRef(currentNode->triangleRef.polygonIndex, static_cast<unsigned int>(currTriangle.getNeighbor(i)));
                    std::shared_ptr<PathNode> neighborNodePath = retrievePathNode(openList, neighborTriangleRef);
                    if(!neighborNodePath)
                    {
                        float gScore = computeGScore(startTriangle, neighborTriangleRef);
                        float hScore = computeHScore(neighborTriangleRef, endTriangle);
                        neighborNodePath.reset(new PathNode(neighborTriangleRef, gScore, hScore));
                        neighborNodePath->previous = currentNode;
                        openList.insert(neighborNodePath);
                    }else
                    {
                        float gScore = computeGScore(startTriangle, neighborTriangleRef);
                        if(neighborNodePath->gScore > gScore)
                        { //better path found to reach neighborNodePath: override previous values
                            neighborNodePath->gScore = gScore;
                            neighborNodePath->previous = currentNode;
                        }
                    }
                }
            }

            if (openList.empty())
            { //no path exists
                //TODO return
            }
        }
    }

    TriangleRef AStar::findTriangle(const Vector3<float> &point) const
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

    bool AStar::isPointInsideTriangle(const Point2<float> &point, const std::shared_ptr<NavPolygon> &polygon, const NavTriangle &triangle) const
    {
        Point3<float> p0 = polygon->getPoint(triangle.getIndex(0));
        Point3<float> p1 = polygon->getPoint(triangle.getIndex(1));
        Point3<float> p2 = polygon->getPoint(triangle.getIndex(2));

        bool b1 = sign(point, Point2<float>(p0.X, p0.Z), Point2<float>(p1.X, p1.Z)) < 0.0f;
        bool b2 = sign(point, Point2<float>(p1.X, p1.Z), Point2<float>(p2.X, p2.Z)) < 0.0f;
        bool b3 = sign(point, Point2<float>(p2.X, p2.Z), Point2<float>(p0.X, p0.Z)) < 0.0f;

        return ((b1 == b2) && (b2 == b3));
    }

    float AStar::sign(const Point2<float> &p1, const Point2<float> &p2, const Point2<float> &p3) const
    {
        return (p1.X - p3.X) * (p2.Y - p3.Y) - (p2.X - p3.X) * (p1.Y - p3.Y);
    }

    float AStar::computeGScore(const TriangleRef &start, const TriangleRef &current) const
    {
        //TODO compute G
    }

    float AStar::computeHScore(const TriangleRef &current, const TriangleRef &end) const
    {
        //TODO compute H
    }

    std::shared_ptr<PathNode> AStar::retrievePathNode(const std::set<std::shared_ptr<PathNode>, PathNodeCompare> &pathNodes, const TriangleRef &triangleRef) const
    {
        for(const auto &pathNode : pathNodes)
        {
            if(pathNode->triangleRef.polygonIndex==triangleRef.polygonIndex && pathNode->triangleRef.triangleIndex==triangleRef.triangleIndex)
            {
                return pathNode;
            }
        }
        return std::shared_ptr<PathNode>();
    }
}
