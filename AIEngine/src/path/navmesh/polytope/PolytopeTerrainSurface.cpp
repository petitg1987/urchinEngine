#include "PolytopeTerrainSurface.h"

namespace urchin
{

    PolytopeTerrainSurface::PolytopeTerrainSurface(const Point3<float> &position, const std::vector<Point3<float>> &localVertices, unsigned int xLength,
                                                   unsigned int zLength, const std::vector<CSGPolygon<float>> &selfObstacles) :
            PolytopeSurface(),
            position(position),
            localVertices(localVertices),
            xLength(xLength),
            zLength(zLength),
            selfObstacles(selfObstacles)
    {
        buildOutlineCwPoints();

        heightfieldPointHelper = std::make_unique<HeightfieldPointHelper<float>>(localVertices, xLength);
    }

    void PolytopeTerrainSurface::buildOutlineCwPoints()
    {
        outlineCwPoints.reserve(4);

        Point3<float> farLeftVertex = position + localVertices[0];
        outlineCwPoints.emplace_back(Point2<float>(farLeftVertex.X, -farLeftVertex.Z));

        Point3<float> farRightVertex = position + localVertices[xLength - 1];
        outlineCwPoints.emplace_back(Point2<float>(farRightVertex.X, -farRightVertex.Z));

        Point3<float> nearRightVertex = position + localVertices[(xLength * zLength) - 1];
        outlineCwPoints.emplace_back(Point2<float>(nearRightVertex.X, -nearRightVertex.Z));

        Point3<float> nearLeftVertex = position + localVertices[(xLength * zLength) - xLength];
        outlineCwPoints.emplace_back(Point2<float>(nearLeftVertex.X, -nearLeftVertex.Z));
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
            minPoint.Y = minPoint.Y > -globalVertex.Z ? -globalVertex.Z : minPoint.Y;

            maxPoint.X = maxPoint.X < globalVertex.X ? globalVertex.X : minPoint.X;
            maxPoint.Y = maxPoint.Y < -globalVertex.Z ? -globalVertex.Z : minPoint.Y;
        }
        return Rectangle<float>(minPoint, maxPoint);
    }

    std::vector<Point2<float>> PolytopeTerrainSurface::getOutlineCwPoints() const
    {
        return outlineCwPoints;
    }

    Plane<float> PolytopeTerrainSurface::getPlane(const Rectangle<float> &box, const NavMeshAgent &agent) const
    {
        Point3<float> point1 = retrieveGlobalVertex(box.getMin());
        Point3<float> point2 = retrieveGlobalVertex(box.getMax());
        Point3<float> point3 = retrieveGlobalVertex(Point2<float>(box.getMin().X, box.getMax().Y));
        Plane<float> plane(point1, point2, point3);

        float expandDistance = agent.computeExpandDistance(plane.getNormal());
        plane.setDistanceToOrigin(plane.getDistanceToOrigin() - expandDistance);

        return plane;
    }

    std::vector<CSGPolygon<float>> PolytopeTerrainSurface::getSelfObstacles() const
    {
        return selfObstacles;
    }

    /**
     * Return point on un-expanded surface
     */
    Point3<float> PolytopeTerrainSurface::computeRealPoint(const Point2<float> &point, const NavMeshAgent &agent) const
    {
        Point3<float> globalVertex = retrieveGlobalVertex(point);
        return Point3<float>(point.X, globalVertex.Y, -point.Y);
    }

    Point3<float> PolytopeTerrainSurface::retrieveGlobalVertex(const Point2<float> &globalXzCoordinate) const
    {
        Point2<float> localCoordinate = Point2<float>(globalXzCoordinate.X - position.X, -globalXzCoordinate.Y - position.Z);
        return heightfieldPointHelper->findPointAt(localCoordinate) + position;
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
