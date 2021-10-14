#pragma once

#include <memory>

#include <Urchin3dEngine.h>
#include <UrchinMapHandler.h>

namespace urchin {

    class BodyShapeDisplayer {
        public:
            explicit BodyShapeDisplayer(Scene&);
            ~BodyShapeDisplayer();

            void setSelectedSceneModel(const SceneModel*);
            void setSelectedCompoundShapeComponent(const LocalizedCollisionShape*);

            void displayBodyShape();
            void clearDisplay();

        private:
            std::unique_ptr<GeometryModel> retrieveSingleGeometry(CollisionShape3D::ShapeType shapeType, const CollisionConvexObject3D&);

            std::unique_ptr<GeometryModel> retrieveSphereGeometry(const CollisionConvexObject3D&);
            std::unique_ptr<GeometryModel> retrieveBoxGeometry(const CollisionConvexObject3D&);
            std::unique_ptr<GeometryModel> retrieveCylinderGeometry(const CollisionConvexObject3D&);
            std::unique_ptr<GeometryModel> retrieveConeGeometry(const CollisionConvexObject3D&);
            std::unique_ptr<GeometryModel> retrieveCapsuleGeometry(const CollisionConvexObject3D&);
            std::unique_ptr<GeometryModel> retrieveConvexHullGeometry(const CollisionConvexObject3D&);

            Scene& scene;
            std::vector<std::shared_ptr<GeometryModel>> bodyShapeModels;

            const SceneModel* selectedSceneModel;
            const LocalizedCollisionShape* selectedCompoundShapeComponent;
    };

}