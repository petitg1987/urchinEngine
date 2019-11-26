#include "EdgeJumpDetection.h"

namespace urchin
{
    void EdgeJumpDetection::detectJump(const LineSegment3D<float> &edge1, const LineSegment3D<float> &edge2, float jumpMaxLength) const
    {
        detectJumpFromEdge1ToEdge2(edge1, edge2, jumpMaxLength);
    }

    /**
     * See '_doc' to understand formulas
     */
    void EdgeJumpDetection::detectJumpFromEdge1ToEdge2(const LineSegment3D<float> &edge1, const LineSegment3D<float> &edge2, float jumpMaxLength) const
    { //TODO rename method + handle division by 0
        Vector3<float> edge1Vector = edge1.getA().vector(edge1.getB());
        float c0Fraction = (edge1Vector.dotProduct(edge2.getB().vector(edge2.getA()))) / edge1Vector.squareLength();
        Vector3<float> c0 = edge2.getB().vector(edge2.getA()) - c0Fraction * edge1Vector;
        float c1Fraction = (edge1Vector.dotProduct(edge1.getA().vector(edge2.getB()))) / edge1Vector.squareLength();
        Vector3<float> c1 = edge1.getA().vector(edge2.getB()) - c1Fraction * edge1Vector;

        float quadraticA = c0.X*c0.X + c0.Y*c0.Y + c0.Z*c0.Z;
        float quadraticB = 2.0f * (c0.X*c1.X + c0.Y*c1.Y + c0.Z*c1.Z);
        float quadraticC = c1.X*c1.X + c1.Y*c1.Y + c1.Z*c1.Z - jumpMaxLength*jumpMaxLength;
        float quadraticSqrt = std::sqrt(quadraticB*quadraticB - 4.0f * quadraticA * quadraticC);

        float r1 = (-quadraticB + quadraticSqrt) / (2.0f * quadraticA);
        float r2 = (-quadraticB - quadraticSqrt) / (2.0f * quadraticA);
        float r1Clamped = std::clamp(r1, 0.0f, 1.0f);
        float r2clamped = std::clamp(r2, 0.0f, 1.0f);

        Point3<float> p1 = r1Clamped*edge2.getA() + (1.0f - r1Clamped) * edge2.getB();
        Point3<float> p2 = r2clamped*edge2.getA() + (1.0f - r2clamped) * edge2.getB();

        std::cout<<std::endl<<"Point 1: "<<p1<<", point 2: "<<p2<<std::endl;
    }

}
