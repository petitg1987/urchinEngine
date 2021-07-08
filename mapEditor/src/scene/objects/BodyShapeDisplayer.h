#pragma once

#include <Urchin3dEngine.h>
#include <UrchinMapHandler.h>

namespace urchin {

    class BodyShapeDisplayer {
        public:
            explicit BodyShapeDisplayer(SceneManager*);
            ~BodyShapeDisplayer();

            void setSelectedSceneObject(const SceneObject*);
            void setSelectedCompoundShapeComponent(const LocalizedCollisionShape*);

            void displayBodyShape();
            void clearDisplay();

        private:
            static GeometryModel* retrieveSingleGeometry(CollisionShape3D::ShapeType shapeType, const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&);

            static GeometryModel* retrieveSphereGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&);
            static GeometryModel* retrieveBoxGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&);
            static GeometryModel* retrieveCylinderGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&);
            static GeometryModel* retrieveConeGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&);
            static GeometryModel* retrieveCapsuleGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&);
            static GeometryModel* retrieveConvexHullGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&);

            SceneManager* sceneManager;
            std::vector<GeometryModel*> bodyShapeModels;

            const SceneObject* selectedSceneObject;
            const LocalizedCollisionShape* selectedCompoundShapeComponent;
    };

}
