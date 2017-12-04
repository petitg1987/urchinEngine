#include "PolytopeTerrainSurface.h"

namespace urchin
{

    PolytopeTerrainSurface::PolytopeTerrainSurface(const Point3<float> &position, const std::vector<Point3<float>> &localVertices,
                                                   unsigned int xLength, unsigned int zLength) :
            position(position),
            localVertices(localVertices),
            xLength(xLength),
            zLength(zLength)
    {
        buildOutlineCwPoints();
    }

    void PolytopeTerrainSurface::buildOutlineCwPoints()
    {
        outlineCwPoints.reserve(4);

        Point3<float> farRightVertex = position + localVertices[0];
        outlineCwPoints.emplace_back(Point2<float>(farRightVertex.X, farRightVertex.Z));

        Point3<float> farLeftVertex = position + localVertices[xLength - 1];
        outlineCwPoints.emplace_back(Point2<float>(farLeftVertex.X, farLeftVertex.Z));

        Point3<float> nearRightVertex = position + localVertices[(xLength * zLength) - xLength];
        outlineCwPoints.emplace_back(Point2<float>(nearRightVertex.X, nearRightVertex.Z));

        Point3<float> nearLeftVertex = position + localVertices[(xLength * zLength) - 1];
        outlineCwPoints.emplace_back(Point2<float>(nearLeftVertex.X, nearLeftVertex.Z));
    }

    bool PolytopeTerrainSurface::isWalkable(float) const
    {
        return isWalkableCandidate();
    }

    Rectangle<float> PolytopeTerrainSurface::computeXZRectangle() const
    {
        Point2<float> minPoint(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Point2<float> maxPoint(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
        for(const auto &localVertex : localVertices)
        {
            Point3<float> globalVertex = position + localVertex;

            minPoint.X = minPoint.X > globalVertex.X ? globalVertex.X : minPoint.X;
            minPoint.Y = minPoint.Y > globalVertex.Z ? globalVertex.Z : minPoint.Y;

            maxPoint.X = maxPoint.X < globalVertex.X ? globalVertex.X : minPoint.X;
            maxPoint.Y = maxPoint.Y < globalVertex.Z ? globalVertex.Z : minPoint.Y;
        }
        return Rectangle<float>(minPoint, maxPoint);
    }

    std::vector<Point2<float>> PolytopeTerrainSurface::getOutlineCwPoints() const
    {
        return outlineCwPoints;
    }

    Plane<float> PolytopeTerrainSurface::getPlaneIn(const Rectangle<float> &box) const
    { //TODO handle shiftDistance & is walkable ?
        Point3<float> point1 = retrieveGlobalVertex(box.getMin());
        Point3<float> point2 = retrieveGlobalVertex(box.getMax());
        Point3<float> point3 = retrieveGlobalVertex(Point2<float>(box.getMin().X, box.getMax().Y));

        return  Plane<float>(point1, point2, point3);
    }

    Point3<float> PolytopeTerrainSurface::elevatePoint(const Point2<float> &point, const NavMeshAgent &navMeshAgent) const
    { //TODO handle shiftDistance
        return retrieveGlobalVertex(point);
    }

    Point3<float> PolytopeTerrainSurface::retrieveGlobalVertex(const Point2<float> &xzCoordinate) const
    { //TODO handle vertices outside terrain
        Point2<float> localXzCoordinate = xzCoordinate - Point2<float>(position.X, position.Z);
        Point2<float> farRightCoordinate = localXzCoordinate - (Point2<float>(localVertices[0].X, localVertices[0].Z));

        float xInterval = localVertices[1].X - localVertices[0].X;
        float zInterval = localVertices[xLength].Z - localVertices[0].Z;

        int xIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farRightCoordinate.X / xInterval)), 0, static_cast<int>(xLength - 1));
        int zIndex = MathAlgorithm::clamp(static_cast<int>(std::round(farRightCoordinate.Y / zInterval)), 0, static_cast<int>(zLength - 1));

        return localVertices[xIndex + zIndex*xLength] + position;
    }

    const Point3<float> &PolytopeTerrainSurface::getPosition() const
    {
        return position;
    }

    const std::vector<Point3<float>> PolytopeTerrainSurface::getLocalVertices() const
    {
        return localVertices;
    }

    unsigned int PolytopeTerrainSurface::getXLength() const
    {
        return xLength;
    }

    unsigned int PolytopeTerrainSurface::getZLength() const
    {
        return zLength;
    }

}
