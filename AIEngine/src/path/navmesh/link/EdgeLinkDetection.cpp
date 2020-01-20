#include <cmath>
#include "EdgeLinkDetection.h"

#define MIN_EDGE_LINK_SQUARE_LENGTH 0.02f

namespace urchin
{
    EdgeLinkDetection::EdgeLinkDetection(float jumpMaxLength) :
            jumpMaxLength(jumpMaxLength),
            jumpMaxSquareLength(jumpMaxLength * jumpMaxLength)
    {
        //TODO no jump between same (split) polygon
    }

    /**
     * @param startEdge Start edge. Edge must be part of a polygon CCW oriented when looked from top
     * @param endEdge End edge. Edge must be part of a polygon CCW oriented when looked from top
     */
    EdgeLinkResult EdgeLinkDetection::detectLink(const LineSegment3D<float> &startEdge, const LineSegment3D<float> &endEdge) const
    {
        Line3D<float> startLine = startEdge.toLine();
        Line3D<float> endLine = endEdge.toLine();

        if(startLine.minDistance(endLine) > jumpMaxLength)
        { //no link between edges
            return EdgeLinkResult::noEdgeLink();
        }

        if(pointsAreEquals(startEdge.getA(), endEdge.getB()) && pointsAreEquals(startEdge.getB(), endEdge.getA()))
        { //identical edges and proper link direction
            return EdgeLinkResult::collinearEdgeLink(1.0f, 0.0f);
        }

        if(isCollinearLines(startLine, endLine))
        { //collinear edges
            float linkStartRange, linkEndRange;
            if(hasCollinearEdgesLink(startEdge, endEdge, linkStartRange, linkEndRange) && startEdge.toVector().dotProduct(endEdge.toVector()) < 0.0f)
            { //collinear edges link and proper link direction
                return EdgeLinkResult::collinearEdgeLink(linkStartRange, linkEndRange);
            }

            return EdgeLinkResult::noEdgeLink();
        }

        const float sampleSpaces = 1.0f;
        const unsigned int samplesCount = 1 + (unsigned int)std::ceil(startEdge.toVector().length() / sampleSpaces);

        bool hasJumpPoints = false;
        float jumpStartRange = -std::numeric_limits<float>::max();
        float jumpEndRange = std::numeric_limits<float>::max();

        for(unsigned int i=0; i<samplesCount; ++i)
        {
            float alpha = (float)i / ((float)samplesCount - 1.0f);
            Point3<float> testPoint = alpha * startEdge.getA() + (1.0f - alpha) * startEdge.getB();
            Point3<float> projectedPoint = endEdge.closestPoint(testPoint);

            if(canJumpThatFar(testPoint, projectedPoint) && isProperJumpDirection(startEdge, endEdge, testPoint, projectedPoint))
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
            if(isRangeTooSmall(jumpStartRange, jumpEndRange, startEdge))
            {
                return EdgeLinkResult::noEdgeLink();
            }
            return EdgeLinkResult::edgeJump(jumpStartRange, jumpEndRange);
        }

        return EdgeLinkResult::noEdgeLink();
    }

    bool EdgeLinkDetection::pointsAreEquals(const Point3<float> &point1, const Point3<float> &point2) const
    {
        return point1.squareDistance(point2) < 0.0001f;
    }

    bool EdgeLinkDetection::isCollinearLines(const Line3D<float> &line1, const Line3D<float> &line2) const
    {
        return line1.squareDistance(line2.getA()) < 0.0001f && line1.squareDistance(line2.getB()) < 0.0001f;
    }

    /**
     * @param touchingStartRange [out] Touching start range
     * @param touchingEndRange [out] Touching end range
     */
    bool EdgeLinkDetection::hasCollinearEdgesLink(const LineSegment3D<float> &startEdge, const LineSegment3D<float> &endEdge,
                                                     float &touchingStartRange, float &touchingEndRange) const
    {
        Point3<float> minIntersection(NAN, NAN, NAN), maxIntersection(NAN, NAN, NAN);
        for(std::size_t i=0; i<3; ++i)
        {
            minIntersection[i] = std::max(std::min(startEdge.getA()[i], startEdge.getB()[i]), std::min(endEdge.getA()[i], endEdge.getB()[i]));
            maxIntersection[i] = std::min(std::max(startEdge.getA()[i], startEdge.getB()[i]), std::max(endEdge.getA()[i], endEdge.getB()[i]));

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
            float denominator = startEdge.getA()[i] - startEdge.getB()[i];
            if(!MathAlgorithm::isZero(denominator))
            {
                touchingStartRange = (minIntersection[i] - startEdge.getB()[i]) / denominator;
                touchingEndRange = (maxIntersection[i] - startEdge.getB()[i]) / denominator;

                if(touchingStartRange < touchingEndRange)
                {
                    std::swap(touchingStartRange, touchingEndRange);
                }

                if(isRangeTooSmall(touchingStartRange, touchingEndRange, startEdge))
                {
                    return false;
                }

                assert(touchingStartRange > -0.0001f && touchingStartRange < 1.0001f);
                assert(touchingEndRange > -0.0001f && touchingEndRange < 1.0001f);

                return true;
            }
        }

        assert(false);
        return false;
    }

    bool EdgeLinkDetection::canJumpThatFar(const Point3<float> &jumpStartPoint, const Point3<float> &jumpEndPoint) const
    {
        return jumpStartPoint.squareDistance(jumpEndPoint) < jumpMaxSquareLength;
    }

    bool EdgeLinkDetection::isProperJumpDirection(const LineSegment3D<float> &startJumpEdge, const LineSegment3D<float> &endJumpEdge,
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

    bool EdgeLinkDetection::isRangeTooSmall(float startRange, float endRange, const LineSegment3D<float> &startEdge) const
    { //if the link on start edge is only one point or a thin line: ignore it for performance and aesthetic reason
        Point3<float> startLinkPoint = startRange * startEdge.getA() + (1.0f - startRange) * startEdge.getB();
        Point3<float> endLinkPoint = endRange * startEdge.getA() + (1.0f - endRange) * startEdge.getB();

        return startLinkPoint.squareDistance(endLinkPoint) < MIN_EDGE_LINK_SQUARE_LENGTH;
    }

}
