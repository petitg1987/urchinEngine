#include "generate/DefaultShapeQuality.h"

namespace urchin {

    //static
    const DefaultShapeQuality DefaultShapeQuality::LOW(0, "Low", -1.0f, 20);
    const DefaultShapeQuality DefaultShapeQuality::MEDIUM(1, "Medium", 0.13f, 25);
    const DefaultShapeQuality DefaultShapeQuality::HIGH(2, "High", 0.07f, 35);
    const DefaultShapeQuality DefaultShapeQuality::ULTRA(3, "Ultra", 0.02f, 50);

    DefaultShapeQuality::DefaultShapeQuality(int id, std::string label, float voxelizationSize, unsigned int convexhullMaxPoints) :
            id(id),
            label(std::move(label)),
            voxelizationSize(voxelizationSize),
            convexHullMaxPoints(convexhullMaxPoints) {

    }

    std::array<DefaultShapeQuality, 4> DefaultShapeQuality::getAllQualities() {
        return {LOW, MEDIUM, HIGH, ULTRA};
    }

    DefaultShapeQuality DefaultShapeQuality::getQualityById(int id) {
        for (const DefaultShapeQuality& quality : getAllQualities()) {
            if (quality.getId() == id) {
                return quality;
            }
        }
        throw std::runtime_error("Unknown default shape quality for id: " + std::to_string(id));
    }

    int DefaultShapeQuality::getId() const {
        return id;
    }

    const std::string& DefaultShapeQuality::getLabel() const {
        return label;
    }

    float DefaultShapeQuality::getVoxelizationSize() const {
        return voxelizationSize;
    }

    unsigned int DefaultShapeQuality::getConvexHullMaxPoints() const {
        return convexHullMaxPoints;
    }

}
