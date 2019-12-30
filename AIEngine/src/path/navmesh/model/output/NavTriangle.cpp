#include <cassert>

#include "NavTriangle.h"
#include "path/navmesh/model/output/NavPolygon.h"

namespace urchin
{

    /**
     * Indices of points in CCW order when looked from top
     */
    NavTriangle::NavTriangle(std::size_t index1, std::size_t index2, std::size_t index3) :
            indices()
    {
        assert(index1!=index2 && index1!=index3 && index2!=index3);

        this->indices[0] = index1;
        this->indices[1] = index2;
        this->indices[2] = index3;

        this->links.reserve(3); //estimated memory size

        this->centerPoint = Point3<float>(0.0f, 0.0f, 0.0f);
    }

    NavTriangle::NavTriangle(const NavTriangle &navTriangle) :
            indices()
    {
        this->indices[0] = navTriangle.getIndex(0);
        this->indices[1] = navTriangle.getIndex(1);
        this->indices[2] = navTriangle.getIndex(2);

        this->links.reserve(3); //estimated memory size

        this->centerPoint = navTriangle.getCenterPoint();
    }

    void NavTriangle::attachNavPolygon(const std::shared_ptr<NavPolygon> &navPolygon)
    {
        this->navPolygon = navPolygon;
        this->centerPoint = (navPolygon->getPoints()[indices[0]] + navPolygon->getPoints()[indices[1]] + navPolygon->getPoints()[indices[2]]) / 3.0f;
    }

    std::shared_ptr<NavPolygon> NavTriangle::getNavPolygon() const
    {
        assert(!navPolygon.expired());

        return navPolygon.lock();
    }

    const Point3<float> &NavTriangle::getCenterPoint() const
    {
        assert(getNavPolygon() != nullptr); //center point not computed until triangle is not linked to polygon

        return centerPoint;
    }

    /**
     * @return Indices of points in CCW order when looked from top
     */
    const std::size_t *NavTriangle::getIndices() const
    {
        return indices;
    }

    /**
     * @return Index of point in CCW order when looked from top
     */
    std::size_t NavTriangle::getIndex(std::size_t index) const
    {
        assert(index <= 2);

        return indices[index];
    }

    void NavTriangle::addDirectLink(std::size_t edgeIndex, const std::shared_ptr<NavTriangle> &targetTriangle)
    {
        #ifndef NDEBUG
            assert(edgeIndex <= 2);
            for(const auto &link : getLinks())
            { //TODO should be a log
                assert(link->getSourceEdgeIndex() != edgeIndex || link->getLinkType() != NavLinkType::DIRECT); //cannot have 2 direct links on same edge
            }
        #endif

        links.emplace_back(NavLink::newDirectLink(edgeIndex, targetTriangle));
    }

    void NavTriangle::addJumpLink(std::size_t edgeIndex, const std::shared_ptr<NavTriangle> &targetTriangle, NavJumpConstraint *jumpConstraint)
    {
        #ifndef NDEBUG
            assert(edgeIndex <= 2);
            for(const auto &link : getLinks())
            { //TODO should be a log
                assert(link->getSourceEdgeIndex() != edgeIndex || link->getLinkType() != NavLinkType::DIRECT); //cannot have a direct links and jump link on same edge
            }
        #endif

        links.emplace_back(NavLink::newJumpLink(edgeIndex, targetTriangle, jumpConstraint));
    }

    void NavTriangle::addLink(const std::shared_ptr<NavLink> &navLink)
    {
        links.emplace_back(navLink);
    }

    void NavTriangle::removeLinksTo(const std::shared_ptr<NavPolygon> &navPolygon)
    {
        links.erase(std::remove_if(links.begin(), links.end(),
                [navPolygon](const std::shared_ptr<NavLink>& link)
                {
                    return link->getTargetTriangle()->getNavPolygon() == navPolygon;
                }), links.end());
    }

    std::vector<std::shared_ptr<NavLink>> NavTriangle::getLinks() const
    {
        return links;
    }

    bool NavTriangle::hasEdgeLinks(std::size_t edgeIndex) const
    {
        for(const auto &link : links)
        {
            if(link->getSourceEdgeIndex() == edgeIndex)
            {
                return true;
            }
        }
        return false;
    }

    bool NavTriangle::isExternalEdge(std::size_t edgeIndex) const
    {
        for(const auto &link : links)
        {
            if(link->getSourceEdgeIndex() == edgeIndex && link->getLinkType() == NavLinkType::DIRECT)
            {
                return false;
            }
        }
        return true;
    }

    LineSegment3D<float> NavTriangle::computeEdge(std::size_t edgeStartIndex) const
    {
        assert(edgeStartIndex <= 2);

        std::size_t edgeEndIndex = (edgeStartIndex + 1) % 3;
        return LineSegment3D<float>(getNavPolygon()->getPoint(indices[edgeStartIndex]), getNavPolygon()->getPoint(indices[edgeEndIndex]));
    }

}
