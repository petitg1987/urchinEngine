#include <cassert>

#include "FunnelAlgorithm.h"

namespace urchin
{

    const std::vector<Point3<float>> &FunnelAlgorithm::determinePath(const std::vector<LineSegment3D<float>> &portals)
    {
        #ifdef _DEBUG
            assert(portals.size() >= 2);
            assert(portals[0].getA() == portals[0].getB()); //degenerated start portal
            assert(portals[portals.size()-1].getA() == portals[portals.size()-1].getB()); //degenerated end portal
        #endif

        const Point3<float> &startPoint = portals[0].getA();

        path.clear();
        path.reserve(portals.size() / 2 + 1); //estimated memory size
        path.push_back(startPoint);

        if(portals.size() <= 2)
        {
            path.push_back(portals[portals.size()-1].getA()); //end point
            return path;
        }else
        {
            apex = startPoint;
            sideIndices = std::make_pair(1, 1);

            for (unsigned int i = 2; i < portals.size(); i++)
            {
                updateFunnelSide(FunnelSide::LEFT, i, portals);
                updateFunnelSide(FunnelSide::RIGHT, i, portals);
            }
        }

        return path;
    }

    void FunnelAlgorithm::updateFunnelSide(FunnelSide updateSide, unsigned int currentIndex, const std::vector<LineSegment3D<float>> &portals)
    {
        FunnelSide otherSide = (updateSide==FunnelSide::LEFT) ? FunnelSide::RIGHT : FunnelSide::LEFT;
        unsigned int sideIndex = (updateSide==FunnelSide::LEFT) ? sideIndices.first : sideIndices.second;
        unsigned int otherSideIndex = (otherSide==FunnelSide::LEFT) ? sideIndices.first : sideIndices.second;

        if(portals[currentIndex].getB() != portals[sideIndex].getB() && currentIndex > sideIndex)
        { //not same point as previous
            Vector3<float> currentSide = apex.vector(portals[sideIndex].getB());
            Vector3<float> newSide = apex.vector(portals[currentIndex].getB());

            float crossProductY = currentSide.Z*newSide.X - currentSide.X*newSide.Z;
            if(crossProductY >= 0.0)
            { //funnel not enlarged
                Vector3<float> currentOtherSide = apex.vector(portals[otherSideIndex].getA());

                crossProductY = currentOtherSide.Z*newSide.X - currentOtherSide.X*newSide.Z;
                if(crossProductY < 0.0)
                { //no cross with other side
                    updateSideIndex(updateSide, currentIndex);
                }else
                { //cross with other side: add new point
                    unsigned int nextOtherSideIndex = otherSideIndex;
                    for (unsigned int j = nextOtherSideIndex; j <= portals.size(); j++)
                    {
                        if (portals[j].getA() != portals[nextOtherSideIndex].getA())
                        {
                            nextOtherSideIndex = j;
                            break;
                        }
                    }

                    path.push_back(portals[otherSideIndex].getA());
                    apex = portals[otherSideIndex].getA();
                    updateSideIndex(otherSide, nextOtherSideIndex);
                }
            }
        }
    }

    void FunnelAlgorithm::updateSideIndex(FunnelSide updateSide, unsigned int newValue)
    {
        if(updateSide==FunnelSide::LEFT)
        {
            sideIndices.first = newValue;
        }else
        {
            sideIndices.second = newValue;
        }
    }
}
