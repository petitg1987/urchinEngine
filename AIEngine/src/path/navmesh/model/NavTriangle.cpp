#include <cassert>

#include "NavTriangle.h"
#include "path/navmesh/model/NavPolygon.h"

namespace urchin
{

    NavTriangle::NavTriangle(std::size_t index1, std::size_t index2, std::size_t index3) :
        indices()
    {
        #ifdef _DEBUG
            assert(index1!=index2 && index1!=index3 && index2!=index3);
        #endif

        this->indices[0] = index1;
        this->indices[1] = index2;
        this->indices[2] = index3;

        this->neighbors[0] = std::shared_ptr<NavTriangle>(nullptr);
        this->neighbors[1] = std::shared_ptr<NavTriangle>(nullptr);
        this->neighbors[2] = std::shared_ptr<NavTriangle>(nullptr);

        this->centerPoint = Point3<float>(0.0f, 0.0f, 0.0f);
    }

    void NavTriangle::attachNavPolygon(const std::shared_ptr<NavPolygon> &navPolygon)
    {
        this->navPolygon = navPolygon;
        this->centerPoint = (navPolygon->getPoints()[indices[0]] + navPolygon->getPoints()[indices[1]] + navPolygon->getPoints()[indices[2]]) / 3.0f;
    }

    std::shared_ptr<NavPolygon> NavTriangle::getNavPolygon() const
    {
        #ifdef _DEBUG
            assert(!navPolygon.expired());
        #endif

        return navPolygon.lock();
    }

    const Point3<float> &NavTriangle::getCenterPoint() const
    {
        #ifdef _DEBUG
            assert(getNavPolygon() != nullptr); //center point not computed until triangle is not linked to polygon
        #endif

        return centerPoint;
    }

    const std::size_t *NavTriangle::getIndices() const
    {
        return indices;
    }

    std::size_t NavTriangle::getIndex(std::size_t index) const
    {
        #ifdef _DEBUG
            assert(index <= 2);
        #endif

        return indices[index];
    }

    void NavTriangle::addNeighbor(std::size_t edgeIndex, const std::shared_ptr<NavTriangle> &navTriangleNeighbor)
    {
        #ifdef _DEBUG
            assert(edgeIndex <= 2);
            assert(getNeighbor(edgeIndex) == nullptr);
        #endif

        neighbors[edgeIndex] = navTriangleNeighbor;
    }

    std::shared_ptr<NavTriangle> NavTriangle::getNeighbor(std::size_t edgeIndex) const
    {
        #ifdef _DEBUG
            assert(edgeIndex <= 2);
        #endif

        return neighbors[edgeIndex].lock();
    }

    LineSegment3D<float> NavTriangle::computeEdge(std::size_t edgeStartIndex) const
    {
        #ifdef _DEBUG
            assert(edgeStartIndex <= 2);
        #endif

        std::size_t edgeEndIndex = (edgeStartIndex + 1) % 3;
        return LineSegment3D<float>(getNavPolygon()->getPoint(indices[edgeStartIndex]), getNavPolygon()->getPoint(indices[edgeEndIndex]));
    }

}
