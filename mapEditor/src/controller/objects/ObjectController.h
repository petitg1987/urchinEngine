#pragma once

#include <list>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinMapHandler.h>

#include <controller/AbstractController.h>

namespace urchin {

    class ObjectController : public AbstractController {
        public:
            ObjectController();

            std::list<const SceneModel*> getSceneModels() const;
            const SceneModel* findSceneModelByBodyId(const std::string&) const;
            void addSceneModel(std::unique_ptr<SceneModel>);
            void removeSceneModel(const SceneModel&);
            void cloneSceneModel(std::unique_ptr<SceneModel>, const SceneModel&);

            void createDefaultBody(const SceneModel&);
            void changeBodyShape(const SceneModel&, CollisionShape3D::ShapeType);
            void removeBody(const SceneModel&);

            const SceneModel& updateSceneModelTransform(const SceneModel&, const Transform<float>&);
            const SceneModel& updateSceneModelFlags(const SceneModel&, bool);
            const SceneModel& updateSceneModelPhysicsProperties(const SceneModel&, float, float, float, float, float, float,
                    const Vector3<float>&, const Vector3<float>&);
            const SceneModel& updateSceneModelPhysicsShape(const SceneModel&, std::unique_ptr<const CollisionShape3D>);

        private:
            SceneModel& findSceneModel(const SceneModel&);
    };

}
