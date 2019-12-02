#include <cmath>
#include "EdgeJumpDetection.h"

namespace urchin
{
    void EdgeJumpDetection::detectJump(const LineSegment3D<float> &edge1, const LineSegment3D<float> &edge2, float jumpMaxLength) const
    { //TODO rename method and handle all cases.
        const LineSegment3D<float> &smallestEdge = edge1.toVector().squareLength() < edge2.toVector().squareLength() ? edge1 : edge2;
        const LineSegment3D<float> &greatestEdge = &smallestEdge==&edge1 ? edge2 : edge1;

        const float sampleSpaces = 1.0f;
        const float jumpMaxSquareLength = jumpMaxLength * jumpMaxLength;
        const unsigned int samplesCount = 1 + (unsigned int)std::ceil(smallestEdge.toVector().length() / sampleSpaces);

        float minAlpha = std::numeric_limits<float>::max();
        float maxAlpha = -std::numeric_limits<float>::max();

        for(unsigned int i=0; i<samplesCount; ++i)
        {
            float alpha = (float)i / ((float)samplesCount - 1.0f);
            Point3<float> testPoint = alpha * smallestEdge.getA() + (1.0f - alpha) * smallestEdge.getB();
            Point3<float> projectedPoint = greatestEdge.closestPoint(testPoint);

            if(testPoint.squareDistance(projectedPoint) < jumpMaxSquareLength)
            {
                minAlpha = std::min(minAlpha, alpha);
                maxAlpha = std::max(maxAlpha, alpha);
            }
        }

        std::cout<<std::endl<<"Point 1 on smallest edge:"<<minAlpha * smallestEdge.getA() + (1.0f - minAlpha) * smallestEdge.getB()<<std::endl;
        std::cout<<"Point 2 on smallest edge:"<<maxAlpha * smallestEdge.getA() + (1.0f - maxAlpha) * smallestEdge.getB()<<std::endl<<std::endl;
    }
}
