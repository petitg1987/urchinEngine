#include <utility>

#include "Polytope.h"
#include "path/navmesh/polytope/PolytopePlaneSurface.h"
#include "path/navmesh/polytope/PolytopeTerrainSurface.h"

namespace urchin {

    /**
     * @param surfaces Indexed faces of the polytope. Surfaces must have their points in counter-clockwise to have face normal pointing outside the polyhedron.
     */
    Polytope::Polytope(std::string name, std::vector<std::shared_ptr<PolytopeSurface>>& surfaces) :
            name(std::move(name)),
            surfaces(std::move(surfaces)),
            walkableCandidate(true),
            obstacleCandidate(true) {
        for (auto& surface : this->surfaces) {
            surface->setPolytope(this);
        }

        buildXZRectangle();
        buildAABBox();
    }

    const std::string& Polytope::getName() const {
        return name;
    }

    const std::vector<std::shared_ptr<PolytopeSurface>>& Polytope::getSurfaces() const {
        return surfaces;
    }

    const std::shared_ptr<PolytopeSurface>& Polytope::getSurface(std::size_t surfaceIndex) const {
        return surfaces[surfaceIndex];
    }

    const Rectangle<float>& Polytope::getXZRectangle() const {
        return xzRectangle;
    }

    const AABBox<float>& Polytope::getAABBox() const {
        return aabbox;
    }

    void Polytope::setWalkableCandidate(bool walkableCandidate) {
        this->walkableCandidate = walkableCandidate;
    }

    bool Polytope::isWalkableCandidate() const {
        return walkableCandidate;
    }

    void Polytope::setObstacleCandidate(bool obstacleCandidate) {
        Polytope::obstacleCandidate = obstacleCandidate;
    }

    bool Polytope::isObstacleCandidate() const {
        return obstacleCandidate;
    }

    void Polytope::buildXZRectangle() {
        xzRectangle = surfaces[0]->computeXZRectangle();
        for (std::size_t i = 1; i < surfaces.size(); ++i) {
            xzRectangle = xzRectangle.merge(surfaces[i]->computeXZRectangle());
        }
    }

    void Polytope::buildAABBox() {
        aabbox = surfaces[0]->getAABBox();
        for (std::size_t i = 1; i < surfaces.size(); ++i) {
            aabbox = aabbox.merge(surfaces[i]->getAABBox());
        }
    }

    std::ostream& operator <<(std::ostream& stream, const Polytope& polytope) {
        unsigned int surfaceIndex = 0;
        for (const auto& surface : polytope.getSurfaces()) {
            stream<<"Surface "<<surfaceIndex++<<" ";
            if (const auto* planeSurface = dynamic_cast<PolytopePlaneSurface*>(surface.get())) {
                for (const auto& point : planeSurface->getCcwPoints()) {
                    stream << "(" << point << ") ";
                }
                stream << std::endl;
            } else if (const auto* terrainSurface = dynamic_cast<PolytopeTerrainSurface*>(surface.get())) {
                terrainSurface->isWalkableCandidate(); //@IgnoreUnused
                stream << "(terrain: " << polytope.getName() << ") " << std::endl;
            }
        }

        return stream;
    }
}
