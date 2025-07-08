#pragma once

#include <model/ObjectEntity.h>

namespace urchin {

    class DefaultBodyShapeGenerator {
        public:
            explicit DefaultBodyShapeGenerator(const ObjectEntity&);

            std::unique_ptr<const CollisionShape3D> generate(CollisionShape3D::ShapeType) const;
            std::unique_ptr<const CollisionShape3D> generate(CollisionShape3D::ShapeType, const std::map<std::string, std::string, std::less<>>&) const;

        private:
            std::unique_ptr<ConvexHullShape3D<float>> buildConvexHullShape() const;
            std::vector<std::shared_ptr<const LocalizedCollisionShape>> buildLocalizedCollisionShapes() const;

            std::vector<std::unique_ptr<LocalizedCollisionShape>> buildBestCollisionShapes(const std::vector<Point3<float>>&) const;

            const ObjectEntity& objectEntity;
    };

}