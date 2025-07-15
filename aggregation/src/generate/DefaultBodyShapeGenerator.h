#pragma once

#include <model/ObjectEntity.h>
#include <generate/DefaultShapeQuality.h>

namespace urchin {

    class DefaultBodyShapeGenerator {
        public:
            explicit DefaultBodyShapeGenerator(const ObjectEntity&, const DefaultShapeQuality&);

            std::unique_ptr<const CollisionShape3D> generate(CollisionShape3D::ShapeType) const;
            std::unique_ptr<const CollisionShape3D> generate(CollisionShape3D::ShapeType, const std::map<std::string, std::string, std::less<>>&) const;

        private:
            std::unique_ptr<ConvexHullShape3D<float>> buildConvexHullShape() const;

            std::vector<std::shared_ptr<const LocalizedCollisionShape>> buildLocalizedCollisionShapes() const;
            std::vector<std::unique_ptr<LocalizedCollisionShape>> buildBestCollisionShapes(std::size_t, const MeshData&) const;

            const ObjectEntity& objectEntity;

            std::unique_ptr<MeshSimplificationService> meshSimplificationService;
            std::unique_ptr<ShapeDetectService> shapeDetectService;
    };

}