#include <resources/material/UvScale.h>
#include <resources/material/UvScaleType.h>

namespace urchin {

    UvScale::UvScale(UvScaleType uScaleType, UvScaleType vScaleType) :
            uScaleType(uScaleType),
            vScaleType(vScaleType) {

    }

    bool UvScale::hasScaling() const {
        return uScaleType != UvScaleType::NONE || vScaleType != UvScaleType::NONE;
    }

    UvScaleType UvScale::getUScaleType() const {
        return uScaleType;
    }

    UvScaleType UvScale::getVScaleType() const {
        return vScaleType;
    }

    float UvScale::scaleU(float uValue, const Vector3<float>& meshScale) const {
        if (uScaleType == UvScaleType::NONE) {
            return uValue;
        } else if (uScaleType == UvScaleType::MESH_SCALE_X) {
            return uValue * meshScale.X;
        } else if (uScaleType == UvScaleType::MESH_SCALE_Y) {
            return uValue * meshScale.Y;
        } else if (uScaleType == UvScaleType::MESH_SCALE_Z) {
            return uValue * meshScale.Z;
        }
        throw std::runtime_error("Unknown UV scale type: " + std::to_string((int)uScaleType));
    }

    float UvScale::scaleV(float vValue, const Vector3<float>& meshScale) const {
        if (vScaleType == UvScaleType::NONE) {
            return vValue;
        } else if (vScaleType == UvScaleType::MESH_SCALE_X) {
            return vValue * meshScale.X;
        } else if (vScaleType == UvScaleType::MESH_SCALE_Y) {
            return vValue * meshScale.Y;
        } else if (vScaleType == UvScaleType::MESH_SCALE_Z) {
            return vValue * meshScale.Z;
        }
        throw std::runtime_error("Unknown UV scale type: " + std::to_string((int)vScaleType));
    }

    bool UvScale::operator ==(const UvScale& uvScale) const {
        return (uScaleType == uvScale.uScaleType && vScaleType == uvScale.vScaleType);
    }

    std::partial_ordering UvScale::operator <=>(const UvScale& uvScale) const {
        if (auto cmpU = uScaleType <=> uvScale.uScaleType; cmpU != 0) {
            return cmpU;
        }
        return vScaleType <=> uvScale.vScaleType;
    }

}
