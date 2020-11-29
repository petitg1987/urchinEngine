#ifndef URCHINENGINE_OBJECTCONTROLLER_H
#define URCHINENGINE_OBJECTCONTROLLER_H

#include <list>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "UrchinMapHandler.h"

#include "controller/AbstractController.h"

namespace urchin {

    class ObjectController : public AbstractController {
        public:
            ObjectController();

            std::list<const SceneObject*> getSceneObjects() const;
            const SceneObject* findSceneObjectByBodyId(const std::string&) const;
            void addSceneObject(SceneObject*);
            void removeSceneObject(const SceneObject*);
            void cloneSceneObject(SceneObject*, const SceneObject*);

            void createDefaultBody(const SceneObject*);
            void changeBodyShape(const SceneObject*, CollisionShape3D::ShapeType);
            void removeBody(const SceneObject*);

            const SceneObject* updateSceneObjectTransform(const SceneObject*, const Transform<float>&);
            const SceneObject* updateSceneObjectFlags(const SceneObject*, bool);
            const SceneObject* updateSceneObjectPhysicsProperties(const SceneObject*, float, float, float, float, float, float,
                    const Vector3<float>&, const Vector3<float>&);
            const SceneObject* updateSceneObjectPhysicsShape(const SceneObject*, const std::shared_ptr< const CollisionShape3D>&);

        private:
            SceneObject* findSceneObject(const SceneObject*);
    };

}

#endif
