#pragma once

#include <model/ObjectEntity.h>

namespace urchin {

    class DefaultBodyShapeGenerator {
        public:
            enum class ShapeQuality {
                LOW = 0,
                MEDIUM = 1,
                HIGH = 2,
                ULTRA = 3
            };

            explicit DefaultBodyShapeGenerator(const ObjectEntity&, ShapeQuality);

            std::unique_ptr<const CollisionShape3D> generate(CollisionShape3D::ShapeType) const;
            std::unique_ptr<const CollisionShape3D> generate(CollisionShape3D::ShapeType, const std::map<std::string, std::string, std::less<>>&) const;

        private:
            std::unique_ptr<ConvexHullShape3D<float>> buildConvexHullShape() const;

            std::vector<std::shared_ptr<const LocalizedCollisionShape>> buildLocalizedCollisionShapes() const;
            std::vector<std::unique_ptr<LocalizedCollisionShape>> buildBestCollisionShapes(std::size_t, const std::vector<Point3<float>>&, const std::vector<std::array<uint32_t, 3>>&) const;

            const ObjectEntity& objectEntity;
            ShapeQuality shapeQuality;
    };

}