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
    {
        Vector3<float> edge1Vector = edge1.getA().vector(edge1.getB());
        Vector3<float> cCommonFraction = ( edge1Vector * edge1Vector ) / (edge1Vector.squareLength());
        Vector3<float> c0 = edge2.getB().vector(edge2.getA()) - (cCommonFraction * (edge2.getB().vector(edge2.getA())));
        Vector3<float> c1 = edge1.getA().vector(edge2.getB()) - (cCommonFraction * (edge1.getA().vector(edge2.getB())));

        float quadraticA = c0.X*c0.X + c0.Y*c0.Y + c0.Z*c0.Z;
        float quadraticB = 2.0f * (c0.X*c1.X + c0.Y*c1.Y + c0.Z*c1.Z);
        float quadraticC = c1.X*c1.X + c1.Y*c1.Y + c1.Z*c1.Z - jumpMaxLength*jumpMaxLength;
        float quadraticSqrt = std::sqrt(quadraticB*quadraticB - 4.0f * quadraticA * quadraticC);

        float r1 = std::clamp((-quadraticB + quadraticSqrt) / (2.0f * quadraticA), 0.0f, 1.0f);
        float r2 = std::clamp((-quadraticB - quadraticSqrt) / (2.0f * quadraticA), 0.0f, 1.0f);

        Point3<float> p1 = r1*edge2.getA() + (1-r1) * edge2.getB();
        Point3<float> p2 = r2*edge2.getA() + (1-r2) * edge2.getB();

        std::cout<<std::endl<<"Point 1: "<<p1<<", point 2: "<<p2<<std::endl;
    }

}
