#include <cmath>
#include "EdgeJumpDetection.h"

namespace urchin
{
    EdgeJumpDetection::EdgeJumpDetection(float jumpMaxLength) :
            jumpMaxLength(jumpMaxLength),
            jumpMaxSquareLength(jumpMaxLength * jumpMaxLength)
    {

    }

    /**
     * @param startJumpEdge Start jump edge. Edge must be part of a polygon CCW oriented when looked from top
     * @param endJumpEdge End jump edge. Edge must be part of a polygon CCW oriented when looked from top
     */
    EdgeJumpResult EdgeJumpDetection::detectJump(const LineSegment3D<float> &startJumpEdge, const LineSegment3D<float> &endJumpEdge) const
    { //TODO handle collinear edges
        if(startJumpEdge.toLine().minDistance(endJumpEdge.toLine()) > jumpMaxLength)
        {
            return EdgeJumpResult::noEdgeJump();
        }

        const float sampleSpaces = 1.0f;
        const unsigned int samplesCount = 1 + (unsigned int)std::ceil(startJumpEdge.toVector().length() / sampleSpaces);

        bool hasJumpPoints = false;
        float jumpStartRange = -std::numeric_limits<float>::max();
        float jumpEndRange = std::numeric_limits<float>::max();

        for(unsigned int i=0; i<samplesCount; ++i)
        {
            float alpha = (float)i / ((float)samplesCount - 1.0f);
            Point3<float> testPoint = alpha * startJumpEdge.getA() + (1.0f - alpha) * startJumpEdge.getB();
            Point3<float> projectedPoint = endJumpEdge.closestPoint(testPoint);

            if(canJumpThatFar(testPoint, projectedPoint) && isProperJumpDirection(startJumpEdge, endJumpEdge, testPoint, projectedPoint))
            {
                hasJumpPoints = true;

                if(alpha > jumpStartRange)
                {
                    jumpStartRange = alpha;
                }

                if(alpha < jumpEndRange)
                {
                    jumpEndRange = alpha;
                }
            }
        }

        if(hasJumpPoints)
        {
            if(jumpStartRange - jumpEndRange < 0.01f)
            { //if the jump start edge is only one point or a thin line: ignore it
                return EdgeJumpResult::noEdgeJump();
            }
            return EdgeJumpResult::edgeJump(jumpStartRange, jumpEndRange);
        }

        return EdgeJumpResult::noEdgeJump();
    }

    bool EdgeJumpDetection::canJumpThatFar(const Point3<float> &jumpStartPoint, const Point3<float> &jumpEndPoint) const
    {
        return jumpStartPoint.squareDistance(jumpEndPoint) < jumpMaxSquareLength;
    }

    bool EdgeJumpDetection::isProperJumpDirection(const LineSegment3D<float> &startJumpEdge, const LineSegment3D<float> &endJumpEdge,
            const Point3<float> &jumpStartPoint, const Point3<float> &jumpEndPoint) const
    {
        constexpr float jumpFoV = 0.17364817766; //FoV of 80° = cos((PI_VALUE/180.0) * 80.0)
        Vector2<float> normalizedJumpVectorXZ = Point2<float>(jumpStartPoint.X, jumpStartPoint.Z).vector(Point2<float>(jumpEndPoint.X, jumpEndPoint.Z)).normalize();

        Line2D<float> startJumpEdgeXZ(Point2<float>(startJumpEdge.getA().X, startJumpEdge.getA().Z), Point2<float>(startJumpEdge.getB().X, startJumpEdge.getB().Z));
        Vector2<float> orthogonalStartJumpVectorXZ = startJumpEdgeXZ.getA().vector(startJumpEdgeXZ.getA().translate(startJumpEdgeXZ.computeNormal()));
        bool jumpOutsideOfStartPolygon = orthogonalStartJumpVectorXZ.normalize().dotProduct(normalizedJumpVectorXZ) >= jumpFoV;

        if(jumpOutsideOfStartPolygon)
        {
            Line2D<float> endJumpEdgeXZ(Point2<float>(endJumpEdge.getA().X, endJumpEdge.getA().Z), Point2<float>(endJumpEdge.getB().X, endJumpEdge.getB().Z));
            Vector2<float> orthogonalEndJumpVectorXZ = endJumpEdgeXZ.getA().vector(endJumpEdgeXZ.getA().translate(endJumpEdgeXZ.computeNormal()));
            bool jumpInsideOfEndPolygon = orthogonalEndJumpVectorXZ.normalize().dotProduct(normalizedJumpVectorXZ) < -jumpFoV;
            return jumpInsideOfEndPolygon;
        }

        return false;
    }
}
