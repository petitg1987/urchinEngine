#include <utility>

#include <path/navmesh/polytope/PolytopeTerrainSurface.h>
#include <path/navmesh/model/output/topography/NavTerrainTopography.h>

namespace urchin {

    PolytopeTerrainSurface::PolytopeTerrainSurface(const Point3<float>& position, std::vector<Point3<float>> localVertices, unsigned int xLength, unsigned int zLength,
            const Vector3<float>& approximateNormal, std::vector<CSGPolygon<float>> selfObstacles, std::shared_ptr<const NavTopography> navTopography) :
            PolytopeSurface(),
            position(position),
            localVertices(std::move(localVertices)),
            xLength(xLength),
            zLength(zLength),
            approximateNormal(approximateNormal),
            selfObstacles(std::move(selfObstacles)),
            navTopography(std::move(navTopography)) {
        assert(this->navTopography);
        buildOutlineCwPoints();
        buildAABBox();

        heightfieldPointHelper = std::make_unique<HeightfieldPointHelper<float>>(this->localVertices, this->xLength);
    }

    void PolytopeTerrainSurface::buildOutlineCwPoints() {
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

    void PolytopeTerrainSurface::buildAABBox() {
        AABBox<float> localAABBox(localVertices);
        aabbox = AABBox<float>(position + localAABBox.getMin(), position + localAABBox.getMax());
    }

    bool PolytopeTerrainSurface::isWalkable() const {
        return isWalkableCandidate();
    }

    Rectangle<float> PolytopeTerrainSurface::computeXZRectangle() const {
        Point3<float> farRightVertex = position + localVertices[xLength - 1];
        Point3<float> nearLeftVertex = position + localVertices[(xLength * zLength) - xLength];

        Point2<float> minPoint(nearLeftVertex.X, -nearLeftVertex.Z);
        Point2<float> maxPoint(farRightVertex.X, -farRightVertex.Z);

        return Rectangle<float>(minPoint, maxPoint);
    }

    const AABBox<float>& PolytopeTerrainSurface::getAABBox() const {
        return aabbox;
    }

    const std::vector<Point2<float>>& PolytopeTerrainSurface::getOutlineCwPoints() const {
        return outlineCwPoints;
    }

    Plane<float> PolytopeTerrainSurface::getPlane(const Rectangle<float>& box) const {
        Point3<float> point1 = retrieveGlobalVertex(box.getMin());
        Point3<float> point2 = retrieveGlobalVertex(box.getMax());
        Point3<float> point3 = retrieveGlobalVertex(Point2<float>(box.getMin().X, box.getMax().Y));
        return Plane<float>(point1, point2, point3);
    }

    const std::vector<CSGPolygon<float>>& PolytopeTerrainSurface::getSelfObstacles() const {
        return selfObstacles;
    }

    /**
     * Return point on un-expanded surface
     */
    Point3<float> PolytopeTerrainSurface::computeRealPoint(const Point2<float>& point, const NavMeshAgent& agent) const {
        Point3<float> expandedPoint = retrieveGlobalVertex(point);

        float reduceDistance = - agent.computeExpandDistance(approximateNormal);
        return expandedPoint.translate(approximateNormal * reduceDistance);
    }

    Point3<float> PolytopeTerrainSurface::retrieveGlobalVertex(const Point2<float>& globalXzCoordinate) const {
        Point2<float> localCoordinate = Point2<float>(globalXzCoordinate.X - position.X, -globalXzCoordinate.Y - position.Z);
        return Point3<float>(localCoordinate.X, heightfieldPointHelper->findHeightAt(localCoordinate), localCoordinate.Y) + position;
    }

    const std::shared_ptr<const NavTopography>& PolytopeTerrainSurface::getNavTopography() const {
        return navTopography;
    }

    const Point3<float>& PolytopeTerrainSurface::getPosition() const {
        return position;
    }

    const std::vector<Point3<float>>& PolytopeTerrainSurface::getLocalVertices() const {
        return localVertices;
    }

    unsigned int PolytopeTerrainSurface::getXLength() const {
        return xLength;
    }

    unsigned int PolytopeTerrainSurface::getZLength() const {
        return zLength;
    }

    const Vector3<float>& PolytopeTerrainSurface::getApproximateNormal() const {
        return approximateNormal;
    }

}
