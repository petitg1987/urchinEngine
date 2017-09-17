#include <cassert>

#include "FunnelAlgorithm.h"

namespace urchin
{

    std::vector<Point3<float>> FunnelAlgorithm::determinePath(const std::vector<LineSegment3D<float>> &portals) const
    {
        #ifdef _DEBUG
            assert(portals.size() >= 2);
            assert(portals[0].getA() == portals[0].getB()); //degenerated start portal
            assert(portals[portals.size()-1].getA() == portals[portals.size()-1].getB()); //degenerated end portal
        #endif

        std::vector<Point3<float>> path;
        path.reserve(portals.size() / 2 + 1); //estimated memory size

        const Point3<float> &startPoint = portals[0].getA();

        if(portals.size() <= 2)
        {
            const Point3<float> &endPoint = portals[portals.size()-1].getA();
            return {startPoint, endPoint};
        }else
        {
            Point3<float> apex = startPoint;
            int leftIndex = 1;
            int rightIndex = 1;

            for (int i = 2; i < portals.size(); i++)
            {
                if(portals[i].getA() != portals[leftIndex].getA() && i > leftIndex)
                { //not same point as previous
                    Vector3<float> currentSide = apex.vector(portals[leftIndex].getA());
                    Vector3<float> newSide = apex.vector(portals[i].getA());

                    float crossProductY = currentSide.Z*newSide.X - currentSide.X*newSide.Z;
                    if(crossProductY >= 0.0)
                    { //funnel not enlarged
                        Vector3<float> currentOtherSide = apex.vector(portals[rightIndex].getB());

                        crossProductY = currentOtherSide.Z*newSide.X - currentOtherSide.X*newSide.Z;
                        if(crossProductY >= 0.0)
                        { //no cross with other side
                            leftIndex = i;
                        }else
                        { //cross with other side: add new point
                            int nextRightIndex= rightIndex;
                            for (int j = nextRightIndex; j <= portals.size(); j++)
                            {
                                if (portals[j].getB() != portals[nextRightIndex].getB())
                                {
                                    nextRightIndex = j;
                                    break;
                                }
                            }

                            path.push_back(portals[rightIndex].getB());
                            apex = portals[rightIndex].getB();
                            rightIndex = nextRightIndex;
                        }
                    }
                }

                //TODO process right

            }
        }

        return path;
    }
}
