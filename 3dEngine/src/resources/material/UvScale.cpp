#include <resources/material/UvScale.h>
#include <resources/material/UvScaleType.h>

namespace urchin {

    UvScale::UvScale(UvScaleType scaleType) :
            scaleType(scaleType) {

    }

    bool UvScale::hasScaling() const {
        return scaleType != UvScaleType::NONE;
    }

    UvScaleType UvScale::getScaleType() const {
        return scaleType;
    }

    Point2<float> UvScale::scaleUV(const Point2<float>& originalUV, const Vector3<float>& meshScale, const Vector3<float>& vertexNormal) const {
        auto isXYFace = [&](){ return std::abs(vertexNormal.dotProduct(Vector3<float>(0.0f, 0.0f, 1.0f))) > 0.99f; };
        auto isXZFace = [&](){ return std::abs(vertexNormal.dotProduct(Vector3<float>(0.0f, 1.0f, 0.0f))) > 0.99f; };
        auto isYZFace = [&](){ return std::abs(vertexNormal.dotProduct(Vector3<float>(1.0f, 0.0f, 1.0f))) > 0.99f; };

        bool scaleOnXYFace = scaleType == UvScaleType::SCALE_ON_AXIS_ALIGNED_FACES || scaleType == UvScaleType::SCALE_ON_XY_FACES;
        bool scaleOnXZFace = scaleType == UvScaleType::SCALE_ON_AXIS_ALIGNED_FACES || scaleType == UvScaleType::SCALE_ON_XZ_FACES;
        bool scaleOnYZFace = scaleType == UvScaleType::SCALE_ON_AXIS_ALIGNED_FACES || scaleType == UvScaleType::SCALE_ON_YZ_FACES;

        if (scaleOnXYFace && isXYFace()) {
            return Point2<float>(originalUV.X * meshScale.X, originalUV.Y * meshScale.Y);
        } else if (scaleOnXZFace && isXZFace()) {
            return Point2<float>(originalUV.X * meshScale.X, originalUV.Y * meshScale.Z);
        } else if (scaleOnYZFace && isYZFace()) {
            return Point2<float>(originalUV.X * meshScale.Y, originalUV.Y * meshScale.Z);
        }
        return originalUV;
    }

    std::partial_ordering UvScale::operator <=>(const UvScale& uvScale) const {
        return scaleType <=> uvScale.scaleType;
    }

}
