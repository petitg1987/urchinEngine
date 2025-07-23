#include <cassert>

#include "path/navmesh/model/output/NavTriangle.h"
#include "path/navmesh/model/output/NavPolygon.h"

namespace urchin {

    /**
     * Indices of points in CCW order when looked from top
     */
    NavTriangle::NavTriangle(std::size_t index1, std::size_t index2, std::size_t index3) :
            indices() {
        assert(index1 != index2 && index1 != index3 && index2 != index3);

        this->indices[0] = index1;
        this->indices[1] = index2;
        this->indices[2] = index3;

        this->links.reserve(3); //estimated memory size

        this->centerPoint = Point3(0.0f, 0.0f, 0.0f);
    }

    NavTriangle::NavTriangle(const NavTriangle& navTriangle) :
            indices(),
            centerPoint(navTriangle.getCenterPoint()) {
        this->indices[0] = navTriangle.getIndex(0);
        this->indices[1] = navTriangle.getIndex(1);
        this->indices[2] = navTriangle.getIndex(2);

        this->links.reserve(3); //estimated memory size
    }

    void NavTriangle::attachNavPolygon(const std::shared_ptr<NavPolygon>& navPolygon) {
        this->navPolygon = navPolygon;
        this->centerPoint = (navPolygon->getPoints()[indices[0]] + navPolygon->getPoints()[indices[1]] + navPolygon->getPoints()[indices[2]]) / 3.0f;
    }

    std::shared_ptr<NavPolygon> NavTriangle::getNavPolygon() const {
        assert(!navPolygon.expired());
        return navPolygon.lock();
    }

    const Point3<float>& NavTriangle::getCenterPoint() const {
        assert(getNavPolygon() != nullptr); //center point not computed until triangle is not linked to polygon
        return centerPoint;
    }

    /**
     * @return Indices of points in CCW order when looked from top
     */
    const std::array<std::size_t, 3>& NavTriangle::getIndices() const {
        return indices;
    }

    /**
     * @return Index of point in CCW order when looked from top
     */
    std::size_t NavTriangle::getIndex(std::size_t index) const {
        assert(index <= 2);

        return indices[index];
    }

    void NavTriangle::addStandardLink(std::size_t sourceEdgeIndex, const std::shared_ptr<NavTriangle>& targetTriangle) {
        addLink(NavLink::newStandardLink(sourceEdgeIndex, targetTriangle));
    }

    void NavTriangle::addJoinPolygonsLink(std::size_t sourceEdgeIndex, const std::shared_ptr<NavTriangle>& targetTriangle, std::unique_ptr<NavLinkConstraint> linkConstraint) {
        addLink(NavLink::newJoinPolygonsLink(sourceEdgeIndex, targetTriangle, std::move(linkConstraint)));
    }

    void NavTriangle::addJumpLink(std::size_t sourceEdgeIndex, const std::shared_ptr<NavTriangle>& targetTriangle, std::unique_ptr<NavLinkConstraint> linkConstraint) {
        addLink(NavLink::newJumpLink(sourceEdgeIndex, targetTriangle, std::move(linkConstraint)));
    }

    void NavTriangle::addLink(const std::shared_ptr<NavLink>& navLink) {
        links.emplace_back(navLink);
        assertLinksValidity();
    }

    void NavTriangle::removeLinksTo(const NavPolygon& navPolygon) {
        std::erase_if(links, [&navPolygon](const auto& link){
            return link->getTargetTriangle()->getNavPolygon().get() == &navPolygon;
        });
    }

    std::vector<std::shared_ptr<NavLink>> NavTriangle::getLinks() const {
        return links;
    }

    bool NavTriangle::hasEdgeLinks(std::size_t edgeIndex) const {
        return std::ranges::any_of(links, [&edgeIndex](const auto& link) {
            return link->getSourceEdgeIndex() == edgeIndex;
        });
    }

    bool NavTriangle::isExternalEdge(std::size_t edgeIndex) const {
        return std::ranges::all_of(links, [&edgeIndex](const auto& link) {
            return link->getSourceEdgeIndex() != edgeIndex || link->getLinkType() != STANDARD;
        });
    }

    LineSegment3D<float> NavTriangle::computeEdge(std::size_t edgeStartIndex) const {
        assert(edgeStartIndex <= 2);

        std::size_t edgeEndIndex = (edgeStartIndex + 1) % 3;
        return LineSegment3D(getNavPolygon()->getPoint(indices[edgeStartIndex]), getNavPolygon()->getPoint(indices[edgeEndIndex]));
    }

    void NavTriangle::assertLinksValidity() const {
        #ifdef URCHIN_DEBUG
            for (unsigned int edgeIndex = 0; edgeIndex < 3; ++edgeIndex) {
                unsigned int countStandardLink = 0;
                unsigned int countJoinPolygonsLink = 0;
                unsigned int countJumpLink = 0;
                for (const auto& link : getLinks()) {
                    assert(link->getSourceEdgeIndex() <= 2);
                    if (link->getSourceEdgeIndex() == edgeIndex) {
                        switch(link->getLinkType()) {
                            case STANDARD: countStandardLink++; break;
                            case JOIN_POLYGONS: countJoinPolygonsLink++; break;
                            case JUMP: countJumpLink++; break;
                            default: break;
                        }
                    }
                }

                assert(countStandardLink <= 1);
                if (countStandardLink > 0) {
                    assert(countJoinPolygonsLink == 0);
                    assert(countJumpLink == 0);
                }
            }
        #endif
    }

}
