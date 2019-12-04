#include <cassert>

#include "NavTriangle.h"
#include "path/navmesh/model/output/NavPolygon.h"

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

        this->links[0] = std::vector<NavLink>();
        this->links[1] = std::vector<NavLink>();
        this->links[2] = std::vector<NavLink>();

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

    void NavTriangle::addDirectLink(std::size_t edgeIndex, const std::shared_ptr<NavTriangle> &targetTriangle)
    {
        #ifdef _DEBUG
            assert(edgeIndex <= 2);
            for(const auto &edgeLink : getEdgeLinks(edgeIndex))
            {
                assert(edgeLink.getLinkType() != NavLink::LinkType::DIRECT); //cannot have 2 direct links on same edge
            }
        #endif

        links[edgeIndex].emplace_back(NavLink(NavLink::LinkType::DIRECT, navPolygon.lock(), targetTriangle));
    }

    void NavTriangle::addJumpLink(std::size_t edgeIndex, const std::shared_ptr<NavPolygon> &targetPolygon, const std::shared_ptr<NavTriangle> &targetTriangle)
    {
        #ifdef _DEBUG
            assert(edgeIndex <= 2);
            for(const auto &edgeLink : getEdgeLinks(edgeIndex))
            {
                assert(edgeLink.getLinkType() != NavLink::LinkType::DIRECT); //cannot have a direct links and jump link on same edge
            }
        #endif

        links[edgeIndex].emplace_back(NavLink(NavLink::LinkType::JUMP, targetPolygon, targetTriangle));
    }

    std::vector<NavLink> NavTriangle::getEdgeLinks(std::size_t edgeIndex) const
    {
        #ifdef _DEBUG
            assert(edgeIndex <= 2);
        #endif

        return links[edgeIndex];
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
