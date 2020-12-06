#include <cmath>
#include "EdgeLinkDetection.h"

namespace urchin {
    EdgeLinkDetection::EdgeLinkDetection(float jumpMaxLength) :
            jumpMaxLength(jumpMaxLength),
            edgeLinkMinLength(ConfigService::instance()->getFloatValue("navMesh.edgeLinkMinLength")),
            equalityDistanceThreshold(ConfigService::instance()->getFloatValue("navMesh.edgeLinkEqualityDistanceThreshold")) {

    }

    /**
     * @param startEdge Start edge. Edge must be part of a polygon CCW oriented when looked from top
     * @param endEdge End edge. Edge must be part of a polygon CCW oriented when looked from top
     */
    EdgeLinkResult EdgeLinkDetection::detectLink(const LineSegment3D<float>& startEdge, const LineSegment3D<float>& endEdge) const {
        Line3D<float> startLine = startEdge.toLine();
        Line3D<float> endLine = endEdge.toLine();

        if (startLine.minDistance(endLine) > jumpMaxLength) { //no link between edges
            return EdgeLinkResult::noEdgeLink();
        }

        if (pointsAreEquals(startEdge.getA(), endEdge.getB()) && pointsAreEquals(startEdge.getB(), endEdge.getA())) { //identical edges and proper link direction
            return EdgeLinkResult::collinearEdgeLink(1.0f, 0.0f);
        }

        if (isCollinearLines(startLine, endLine)) { //collinear edges
            float linkStartRange, linkEndRange;
            if (hasCollinearEdgesLink(startEdge, endEdge, linkStartRange, linkEndRange) && startEdge.toVector().dotProduct(endEdge.toVector()) < 0.0f) { //collinear edges link and proper link direction
                return EdgeLinkResult::collinearEdgeLink(linkStartRange, linkEndRange);
            }

            return EdgeLinkResult::noEdgeLink();
        }

        const float sampleSpaces = 1.0f;
        const unsigned int samplesCount = 1 + (unsigned int)std::ceil(startEdge.toVector().length() / sampleSpaces);

        bool hasJumpPoints = false;
        float jumpStartRange = -std::numeric_limits<float>::max();
        float jumpEndRange = std::numeric_limits<float>::max();

        for (unsigned int i=0; i<samplesCount; ++i) {
            float alpha = (float)i / ((float)samplesCount - 1.0f);
            Point3<float> testPoint = alpha * startEdge.getA() + (1.0f - alpha) * startEdge.getB();
            Point3<float> projectedPoint = endEdge.closestPoint(testPoint);

            if (canJumpThatFar(testPoint, projectedPoint) && isProperJumpDirection(startEdge, endEdge, testPoint, projectedPoint)) {
                hasJumpPoints = true;

                if (alpha > jumpStartRange) {
                    jumpStartRange = alpha;
                }

                if (alpha < jumpEndRange) {
                    jumpEndRange = alpha;
                }
            }
        }

        if (hasJumpPoints) {
            if (isRangeTooSmall(jumpStartRange, jumpEndRange, startEdge)) {
                return EdgeLinkResult::noEdgeLink();
            }
            return EdgeLinkResult::edgeJump(jumpStartRange, jumpEndRange);
        }

        return EdgeLinkResult::noEdgeLink();
    }

    bool EdgeLinkDetection::pointsAreEquals(const Point3<float>& point1, const Point3<float>& point2) const {
        return point1.squareDistance(point2) < equalityDistanceThreshold * equalityDistanceThreshold;
    }

    bool EdgeLinkDetection::isCollinearLines(const Line3D<float>& line1, const Line3D<float>& line2) const {
        return line1.squareDistance(line2.getA()) < equalityDistanceThreshold * equalityDistanceThreshold
            && line1.squareDistance(line2.getB()) < equalityDistanceThreshold * equalityDistanceThreshold;
    }

    /**
     * @param touchingStartRange [out] Touching start range
     * @param touchingEndRange [out] Touching end range
     */
    bool EdgeLinkDetection::hasCollinearEdgesLink(const LineSegment3D<float>& startEdge, const LineSegment3D<float>& endEdge,
                                                     float& touchingStartRange, float& touchingEndRange) const {
        Point3<float> minIntersection(NAN, NAN, NAN), maxIntersection(NAN, NAN, NAN);
        for (std::size_t i=0; i<3; ++i) {
            minIntersection[i] = std::max(std::min(startEdge.getA()[i], startEdge.getB()[i]), std::min(endEdge.getA()[i], endEdge.getB()[i]));
            maxIntersection[i] = std::min(std::max(startEdge.getA()[i], startEdge.getB()[i]), std::max(endEdge.getA()[i], endEdge.getB()[i]));

            if (minIntersection[i] > maxIntersection[i] + equalityDistanceThreshold) { //collinear edges are not touching each other
                touchingStartRange = NAN;
                touchingEndRange = NAN;
                return false;
            }
        }

        touchingStartRange = 1.0f;
        touchingEndRange = 0.0f;
        for (std::size_t i=0; i<3; ++i) {
            float denominator = startEdge.getA()[i] - startEdge.getB()[i];
            if (!MathFunction::isZero(denominator)) {
                touchingStartRange = std::clamp((minIntersection[i] - startEdge.getB()[i]) / denominator, 0.0f, 1.0f);
                touchingEndRange = std::clamp((maxIntersection[i] - startEdge.getB()[i]) / denominator, 0.0f, 1.0f);

                if (touchingStartRange < touchingEndRange) {
                    std::swap(touchingStartRange, touchingEndRange);
                }

                return !isRangeTooSmall(touchingStartRange, touchingEndRange, startEdge);
            }
        }

        assert(false);
        return false;
    }

    bool EdgeLinkDetection::canJumpThatFar(const Point3<float>& jumpStartPoint, const Point3<float>& jumpEndPoint) const {
        return jumpStartPoint.squareDistance(jumpEndPoint) < jumpMaxLength * jumpMaxLength;
    }

    bool EdgeLinkDetection::isProperJumpDirection(const LineSegment3D<float>& startJumpEdge, const LineSegment3D<float>& endJumpEdge,
                                                  const Point3<float>& jumpStartPoint, const Point3<float>& jumpEndPoint) const {
        constexpr float jumpFoV = 0.17364817766f; //FoV of 80° = cos((MathValue::PI_FLOAT / 180.0f) * 80.0f)
        Vector2<float> normalizedJumpVectorXZ = jumpStartPoint.toPoint2XZ().vector(jumpEndPoint.toPoint2XZ()).normalize();

        Line2D<float> startJumpEdgeXZ(startJumpEdge.getA().toPoint2XZ(), startJumpEdge.getB().toPoint2XZ());
        Vector2<float> orthogonalStartJumpVectorXZ = startJumpEdgeXZ.getA().vector(startJumpEdgeXZ.getA().translate(startJumpEdgeXZ.computeNormal()));
        bool jumpOutsideOfStartPolygon = orthogonalStartJumpVectorXZ.normalize().dotProduct(normalizedJumpVectorXZ) >= jumpFoV;

        if (jumpOutsideOfStartPolygon) {
            Line2D<float> endJumpEdgeXZ(endJumpEdge.getA().toPoint2XZ(), endJumpEdge.getB().toPoint2XZ());
            Vector2<float> orthogonalEndJumpVectorXZ = endJumpEdgeXZ.getA().vector(endJumpEdgeXZ.getA().translate(endJumpEdgeXZ.computeNormal()));
            bool jumpInsideOfEndPolygon = orthogonalEndJumpVectorXZ.normalize().dotProduct(normalizedJumpVectorXZ) < -jumpFoV;
            return jumpInsideOfEndPolygon;
        }

        return false;
    }

    bool EdgeLinkDetection::isRangeTooSmall(float startRange, float endRange, const LineSegment3D<float>& startEdge) const { //if the link on start edge is only one point or a thin line: ignore it for performance and aesthetic reason
        Point3<float> startLinkPoint = startRange * startEdge.getA() + (1.0f - startRange) * startEdge.getB();
        Point3<float> endLinkPoint = endRange * startEdge.getA() + (1.0f - endRange) * startEdge.getB();

        return startLinkPoint.squareDistance(endLinkPoint) < edgeLinkMinLength * edgeLinkMinLength;
    }

}
