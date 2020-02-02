#include <cassert>

#include "PlaneSurfaceSplitService.h"

namespace urchin
{
    PlaneSurfaceSplitService::PlaneSurfaceSplitService(float surfaceMaxSize) :
            surfaceMaxSize(surfaceMaxSize)
    {

    }

    std::vector<PlaneSurfaceSplit> PlaneSurfaceSplitService::splitSurface(const std::vector<Point3<float>> &planeSurfacePoints)
    { //TODO divide big surface in several smallest surfaces of 'surfaceMaxSize' size
        assert(planeSurfacePoints.size() == 4);

        std::vector<PlaneSurfaceSplit> planeSurfaceSplits;

        PlaneSurfaceSplit planeSurfaceSplit;
        planeSurfaceSplit.planeSurfacePoints = planeSurfacePoints;
        planeSurfaceSplits.emplace_back(planeSurfaceSplit);

        return planeSurfaceSplits;
    }
}
