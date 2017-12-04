#include <cassert>

#include "PolytopeTerrainSurface.h"

namespace urchin
{

    PolytopeTerrainSurface::PolytopeTerrainSurface(const Transform<float> &transform, const std::vector<Point3<float>> &localVertices,
                                                   unsigned int xLength, unsigned int zLength) :
            transform(transform),
            localVertices(localVertices),
            xLength(xLength),
            zLength(zLength)
    {
        #ifdef _DEBUG
            assert(MathAlgorithm::isOne(transform.getScale()));
        #endif

        buildOutlineCwPoints();
    }

    void PolytopeTerrainSurface::buildOutlineCwPoints()
    {
        outlineCwPoints.reserve(4);

        Point3<float> farRightVertex = transform.getOrientation().rotatePoint(localVertices[0]) + transform.getPosition();
        outlineCwPoints.emplace_back(Point2<float>(farRightVertex.X, farRightVertex.Z));

        Point3<float> farLeftVertex = transform.getOrientation().rotatePoint(localVertices[xLength - 1]) + transform.getPosition();
        outlineCwPoints.emplace_back(Point2<float>(farLeftVertex.X, farLeftVertex.Z));

        Point3<float> nearRightVertex = transform.getOrientation().rotatePoint(localVertices[(xLength * zLength) - xLength]) + transform.getPosition();
        outlineCwPoints.emplace_back(Point2<float>(nearRightVertex.X, nearRightVertex.Z));

        Point3<float> nearLeftVertex = transform.getOrientation().rotatePoint(localVertices[(xLength * zLength) - 1]) + transform.getPosition();
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
        for(const auto &vertex : localVertices)
        {
            Point3<float> transformedVertex = transform.getOrientation().rotatePoint(vertex) + transform.getPosition();

            minPoint.X = minPoint.X > transformedVertex.X ? transformedVertex.X : minPoint.X;
            minPoint.Y = minPoint.Y > transformedVertex.Z ? transformedVertex.Z : minPoint.Y;

            maxPoint.X = maxPoint.X < transformedVertex.X ? transformedVertex.X : minPoint.X;
            maxPoint.Y = maxPoint.Y < transformedVertex.Z ? transformedVertex.Z : minPoint.Y;
        }
        return Rectangle<float>(minPoint, maxPoint);
    }

    std::vector<Point2<float>> PolytopeTerrainSurface::getOutlineCwPoints() const
    {
        return outlineCwPoints;
    }

    Plane<float> PolytopeTerrainSurface::getPlaneIn(const Rectangle<float> &box) const
    {
        Plane<float> plane;

        //TODO create plane + check if walkable + /!\ shift distance

        return plane;
    }

    Point3<float> PolytopeTerrainSurface::elevatePoint(const Point2<float> &point, const NavMeshAgent &navMeshAgent) const
    {
        Point3<float> elevatedPoint;

        //TODO compute elevatePoint

        return elevatedPoint;
    }

    const Transform<float> &PolytopeTerrainSurface::getTransform() const
    {
        return transform;
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
