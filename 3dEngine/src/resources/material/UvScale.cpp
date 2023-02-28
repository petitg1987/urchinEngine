#include <resources/material/UvScale.h>

namespace urchin {

    UvScale::UvScale(UvScaleType scaleType, MeshScaleAxis uMeshScaleAxis, MeshScaleAxis vMeshScaleAxis) :
            scaleType(scaleType),
            uMeshScaleAxis(uMeshScaleAxis),
            vMeshScaleAxis(vMeshScaleAxis) {

    }

    bool UvScale::hasScaling() const {
        if (scaleType == UvScaleType::SCALE_ON_MESH_SCALE) {
            return uMeshScaleAxis != MeshScaleAxis::NONE || vMeshScaleAxis != MeshScaleAxis::NONE;
        }
        return scaleType != UvScaleType::NONE;
    }

    Point2<float> UvScale::scaleUV(const Point2<float>& originalUV, const Vector3<float>& meshScale, const Vector3<float>& vertexNormal) const {
        if (scaleType == UvScaleType::SCALE_ON_MESH_SCALE) {
            float uScaled = originalUV.X * ((uMeshScaleAxis == MeshScaleAxis::NONE) ? 1.0f : meshScale[(std::size_t)uMeshScaleAxis]);
            float vScaled = originalUV.Y * ((vMeshScaleAxis == MeshScaleAxis::NONE) ? 1.0f : meshScale[(std::size_t)vMeshScaleAxis]);
            return Point2<float>(uScaled, vScaled);
        } else {
            auto isXYFace = [&]() { return std::abs(vertexNormal.dotProduct(Vector3<float>(0.0f, 0.0f, 1.0f))) > 0.99f; };
            auto isXZFace = [&]() { return std::abs(vertexNormal.dotProduct(Vector3<float>(0.0f, 1.0f, 0.0f))) > 0.99f; };
            auto isYZFace = [&]() { return std::abs(vertexNormal.dotProduct(Vector3<float>(1.0f, 0.0f, 1.0f))) > 0.99f; };

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
    }

    std::partial_ordering UvScale::operator <=>(const UvScale& uvScale) const {
        return scaleType <=> uvScale.scaleType;
    }

}
