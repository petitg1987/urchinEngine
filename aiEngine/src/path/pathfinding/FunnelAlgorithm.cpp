#include <cassert>
#include <utility>

#include <path/pathfinding/FunnelAlgorithm.h>

namespace urchin {

    /**
     * @param portals List of portals to cross by the character to reach his target.
     * First point (getA()) of portal segment must be on left of character when it cross a portal.
     */
    void FunnelAlgorithm::computePivotPoints(const std::vector<std::unique_ptr<PathPortal>>& portals) {
        #ifdef URCHIN_DEBUG
            assert(portals.size() >= 2);
            assert(portals[0]->getPortal().getA() == portals[0]->getPortal().getB()); //degenerated start portal
            assert(portals.back()->getPortal().getA() == portals.back()->getPortal().getB()); //degenerated end portal
        #endif

        const Point3<float>& startPoint = portals[0]->getPortal().getA();
        const Point3<float>& endPoint = portals.back()->getPortal().getA();

        portals[0]->setTransitionPoint(startPoint);

        apex = startPoint;
        sideIndices = std::make_pair(1, 1);

        for (std::size_t portalIndex = 2; portalIndex < portals.size(); portalIndex++) {
            for (FunnelSide funnelSide : {LEFT, RIGHT}) {
                int newPortalIndex = updateFunnelSide(portals, funnelSide, (unsigned int)portalIndex);
                if (newPortalIndex != -1) {
                    portalIndex = (unsigned int)newPortalIndex;
                    break;
                }
            }
        }

        portals.back()->setTransitionPoint(endPoint);
    }

    int FunnelAlgorithm::updateFunnelSide(const std::vector<std::unique_ptr<PathPortal>>& portals, FunnelSide updateSide, unsigned int currentIndex) {
        FunnelSide otherSide = (updateSide == LEFT) ? RIGHT : LEFT;
        unsigned int sideIndex = (updateSide == LEFT) ? sideIndices.first : sideIndices.second;
        unsigned int otherSideIndex = (otherSide == LEFT) ? sideIndices.first : sideIndices.second;

        if (getPortalPoint(portals, updateSide, currentIndex) != getPortalPoint(portals, updateSide, sideIndex) && currentIndex > sideIndex) { //not same point as previous
            Vector3<float> currentSide = apex.vector(getPortalPoint(portals, updateSide, sideIndex));
            Vector3<float> newSide = apex.vector(getPortalPoint(portals, updateSide, currentIndex));

            float crossProductY = currentSide.Z * newSide.X - currentSide.X * newSide.Z;
            if ((updateSide == LEFT && crossProductY <= 0.0) || (updateSide == RIGHT && crossProductY >= 0.0)) { //funnel not enlarged
                Vector3<float> currentOtherSide = apex.vector(getPortalPoint(portals, otherSide, otherSideIndex));

                crossProductY = currentOtherSide.Z * newSide.X - currentOtherSide.X * newSide.Z;
                if ((updateSide == LEFT && crossProductY >= 0.0) || (updateSide == RIGHT && crossProductY <= 0.0)) { //no cross with other side
                    updateSideIndex(updateSide, currentIndex);
                } else { //cross with other side: add new point
                    apex = getPortalPoint(portals, otherSide, otherSideIndex);
                    portals[otherSideIndex]->setTransitionPoint(apex);

                    updateSideIndex(otherSide, otherSideIndex + 1);
                    updateSideIndex(updateSide, otherSideIndex + 1);

                    return (int)otherSideIndex + 1;
                }
            }
        }

        return -1;
    }

    const Point3<float>& FunnelAlgorithm::getPortalPoint(const std::vector<std::unique_ptr<PathPortal>>& portals, FunnelSide updateSide, unsigned int index) const {
        if (updateSide == LEFT) {
            return portals[index]->getPortal().getA();
        } else {
            return portals[index]->getPortal().getB();
        }
    }

    void FunnelAlgorithm::updateSideIndex(FunnelSide updateSide, unsigned int newValue) {
        if (updateSide == LEFT) {
            sideIndices.first = newValue;
        } else {
            sideIndices.second = newValue;
        }
    }
}
