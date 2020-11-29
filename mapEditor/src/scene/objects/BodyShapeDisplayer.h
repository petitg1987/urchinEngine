#ifndef URCHINENGINE_BODYSHAPEDISPLAYER_H
#define URCHINENGINE_BODYSHAPEDISPLAYER_H

#include "Urchin3dEngine.h"
#include "UrchinMapHandler.h"

namespace urchin {

    class BodyShapeDisplayer {
        public:
            explicit BodyShapeDisplayer(SceneManager*);
            ~BodyShapeDisplayer();

            void setSelectedSceneObject(const SceneObject*);
            void setSelectedCompoundShapeComponent(std::shared_ptr<const LocalizedCollisionShape>);

            void displayBodyShape();

        private:
            GeometryModel* retrieveSingleGeometry(CollisionShape3D::ShapeType shapeType, const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&);

            GeometryModel* retrieveSphereGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&) const;
            GeometryModel* retrieveBoxGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&) const;
            GeometryModel* retrieveCylinderGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&) const;
            GeometryModel* retrieveConeGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&) const;
            GeometryModel* retrieveCapsuleGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&) const;
            GeometryModel* retrieveConvexHullGeometry(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&) const;

            void cleanCurrentDisplay();

            SceneManager *sceneManager;
            std::vector<GeometryModel *> bodyShapeModels;

            const SceneObject *selectedSceneObject;
            std::shared_ptr<const LocalizedCollisionShape> selectedCompoundShapeComponent;
    };

}

#endif
