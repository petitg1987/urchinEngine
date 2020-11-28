#include "PlaneSurfaceSplitService.h"

namespace urchin {
    PlaneSurfaceSplitService::PlaneSurfaceSplitService(float surfaceMaxSize) :
            surfaceMaxSize(surfaceMaxSize) {

    }

    std::vector<PlaneSurfaceSplit> PlaneSurfaceSplitService::splitRectangleSurface(const std::vector<Point3<float>> &planeSurfacePoints) const {
        #ifndef NDEBUG
            assert(planeSurfacePoints.size() == 4);
            assert(MathAlgorithm::isZero(planeSurfacePoints[0].distance(planeSurfacePoints[1]) - planeSurfacePoints[2].distance(planeSurfacePoints[3]), 0.01f));
            assert(MathAlgorithm::isZero(planeSurfacePoints[1].distance(planeSurfacePoints[2]) - planeSurfacePoints[3].distance(planeSurfacePoints[0]), 0.01f));
        #endif

        std::vector<PlaneSurfaceSplit> planeSurfaceSplits;

        auto aSamples = static_cast<unsigned int>(std::ceil(planeSurfacePoints[0].distance(planeSurfacePoints[1]) / surfaceMaxSize));
        auto bSamples = static_cast<unsigned int>(std::ceil(planeSurfacePoints[1].distance(planeSurfacePoints[2]) / surfaceMaxSize));

        if (aSamples == 1 && bSamples == 1) { //no split required
            PlaneSurfaceSplit planeSurfaceSplit;
            planeSurfaceSplit.planeSurfacePoints = planeSurfacePoints;
            planeSurfaceSplits.emplace_back(planeSurfaceSplit);

            return planeSurfaceSplits;
        }

        for (unsigned int aSample=0; aSample < aSamples; ++aSample) {
            float aPointStartRange = (float)aSample / (float)aSamples;
            LineSegment3D<float> aLine((1.0f - aPointStartRange) * planeSurfacePoints[0] + aPointStartRange * planeSurfacePoints[1],
                                            (1.0f - aPointStartRange) * planeSurfacePoints[3] + aPointStartRange * planeSurfacePoints[2]);
            float aPointEndRange = (float)(aSample + 1) / (float)aSamples;
            LineSegment3D<float> aNextLine((1.0f - aPointEndRange) * planeSurfacePoints[0] + aPointEndRange * planeSurfacePoints[1],
                                            (1.0f - aPointEndRange) * planeSurfacePoints[3] + aPointEndRange * planeSurfacePoints[2]);

            for (unsigned int bSample = 0; bSample < bSamples; ++bSample) {
                float bPointStartRange = (float)bSample / (float)bSamples;
                float bPointEndRange = (float)(bSample + 1) / (float)bSamples;

                Point3<float> firstPoint = (1.0f - bPointStartRange) * aLine.getA() + bPointStartRange * aLine.getB();
                Point3<float> secondPoint = (1.0f - bPointStartRange) * aNextLine.getA() + bPointStartRange * aNextLine.getB();
                Point3<float> thirdPoint = (1.0f - bPointEndRange) * aNextLine.getA() + bPointEndRange * aNextLine.getB();
                Point3<float> fourthPoint = (1.0f - bPointEndRange) * aLine.getA() + bPointEndRange * aLine.getB();

                PlaneSurfaceSplit planeSurfaceSplit;
                planeSurfaceSplit.planeSurfacePoints = {firstPoint, secondPoint, thirdPoint, fourthPoint};
                planeSurfaceSplits.emplace_back(planeSurfaceSplit);
            }
        }

        return planeSurfaceSplits;
    }
}
