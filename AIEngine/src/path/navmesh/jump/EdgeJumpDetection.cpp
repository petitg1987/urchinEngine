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
    {
        Line3D<float> startJumpLine = startJumpEdge.toLine();
        Line3D<float> endJumpLine = endJumpEdge.toLine();

        if(startJumpLine.minDistance(endJumpLine) > jumpMaxLength)
        { //no jump between edges
            return EdgeJumpResult::noEdgeJump();
        }

        if(pointsAreEquals(startJumpEdge.getA(), endJumpEdge.getB()) && pointsAreEquals(startJumpEdge.getB(), endJumpEdge.getA()))
        { //identical edges and proper jump direction
            return EdgeJumpResult::collinearEdgeJump(1.0f, 0.0f);
        }

        if(isCollinearLines(startJumpLine, endJumpLine))
        { //collinear edges
            float jumpStartRange, jumpEndRange;
            if(isTouchingCollinearEdges(startJumpEdge, endJumpEdge, jumpStartRange, jumpEndRange)
                    && startJumpEdge.toVector().dotProduct(endJumpEdge.toVector()) < 0.0f)
            { //touching collinear edges and proper jump direction
                assert(jumpStartRange > -0.0001f && jumpStartRange < 1.0001f);
                assert(jumpEndRange > -0.0001f && jumpEndRange < 1.0001f);

                return EdgeJumpResult::collinearEdgeJump(jumpStartRange, jumpEndRange);
            }

            return EdgeJumpResult::collinearNoEdgeJump();
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

    bool EdgeJumpDetection::pointsAreEquals(const Point3<float> &point1, const Point3<float> &point2) const
    {
        return point1.squareDistance(point2) < 0.0001f;
    }

    bool EdgeJumpDetection::isCollinearLines(const Line3D<float> &line1, const Line3D<float> &line2) const
    {
        return line1.squareDistance(line2.getA()) < 0.0001f && line1.squareDistance(line2.getB()) < 0.0001f;
    }

    /**
     * @param touchingStartRange [out] Touching start range
     * @param touchingEndRange [out] Touching end range
     */
    bool EdgeJumpDetection::isTouchingCollinearEdges(const LineSegment3D<float> &startJumpEdge, const LineSegment3D<float> &endJumpEdge,
                                                     float &touchingStartRange, float &touchingEndRange) const
    {
        Point3<float> minIntersection(NAN, NAN, NAN), maxIntersection(NAN, NAN, NAN);
        for(std::size_t i=0; i<3; ++i)
        {
            minIntersection[i] = std::max(std::min(startJumpEdge.getA()[i], startJumpEdge.getB()[i]), std::min(endJumpEdge.getA()[i], endJumpEdge.getB()[i]));
            maxIntersection[i] = std::min(std::max(startJumpEdge.getA()[i], startJumpEdge.getB()[i]), std::max(endJumpEdge.getA()[i], endJumpEdge.getB()[i]));

            if(minIntersection[i] > maxIntersection[i] + 0.0001f)
            { //collinear edges are not touching each other
                touchingStartRange = NAN;
                touchingEndRange = NAN;
                return false;
            }
        }

        touchingStartRange = 1.0f;
        touchingEndRange = 0.0f;
        for(std::size_t i=0; i<3; ++i)
        {
            float denominator = startJumpEdge.getA()[i] - startJumpEdge.getB()[i];
            if(!MathAlgorithm::isZero(denominator))
            {
                touchingStartRange = (minIntersection[i] - startJumpEdge.getB()[i]) / denominator;
                touchingEndRange = (maxIntersection[i] - startJumpEdge.getB()[i]) / denominator;
                break;
            }
        }
        return true;
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
