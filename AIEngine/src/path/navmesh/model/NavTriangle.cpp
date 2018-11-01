#include <cassert>

#include "NavTriangle.h"
#include "path/navmesh/model/NavPolygon.h"

namespace urchin
{

    NavTriangle::NavTriangle(unsigned int index1, unsigned int index2, unsigned int index3)
    {
        #ifdef _DEBUG
            assert(index1!=index2 && index1!=index3 && index2!=index3);
        #endif

        this->indices[0] = index1;
        this->indices[1] = index2;
        this->indices[2] = index3;

        this->neighbors[0] = nullptr;
        this->neighbors[1] = nullptr;
        this->neighbors[2] = nullptr;

        this->centerPoint = Point3<float>(0.0, 0.0, 0.0);
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

    const unsigned int *NavTriangle::getIndices() const
    {
        return indices;
    }

    unsigned int NavTriangle::getIndex(unsigned int index) const
    {
        #ifdef _DEBUG
            assert(index <= 2);
        #endif

        return indices[index];
    }

    void NavTriangle::addNeighbor(unsigned int edgeIndex, const std::shared_ptr<NavTriangle> &navTriangleNeighbor)
    {
        #ifdef _DEBUG
            assert(edgeIndex <= 2);
            assert(neighbors[edgeIndex] == nullptr);
        #endif

        neighbors[edgeIndex] = navTriangleNeighbor;
    }

    const std::shared_ptr<NavTriangle> &NavTriangle::getNeighbor(unsigned int edgeIndex) const
    {
        #ifdef _DEBUG
            assert(edgeIndex <= 2);
        #endif

        return neighbors[edgeIndex];
    }

    LineSegment3D<float> NavTriangle::computeEdge(unsigned int edgeStartIndex) const
    {
        #ifdef _DEBUG
            assert(edgeStartIndex <= 2);
        #endif

        unsigned int edgeEndIndex = (edgeStartIndex + 1) % 3;
        return LineSegment3D<float>(getNavPolygon()->getPoint(indices[edgeStartIndex]), getNavPolygon()->getPoint(indices[edgeEndIndex]));
    }

}
