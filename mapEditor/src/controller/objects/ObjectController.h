#pragma once

#include <list>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinAggregation.h>

#include <controller/AbstractController.h>

namespace urchin {

    class ObjectController final : public AbstractController {
        public:
            static constexpr char TAGS_SEPARATOR = ',';

            std::list<const ObjectEntity*> getObjectEntities() const;
            const ObjectEntity* findObjectEntityByBodyId(std::string_view) const;
            ObjectEntity& addObjectEntity(std::unique_ptr<ObjectEntity>);
            void removeObjectEntity(const ObjectEntity&);
            ObjectEntity& cloneObjectEntity(std::string, const ObjectEntity&);
            void renameObjectEntity(const ObjectEntity&, std::string);
            bool moveUpObjectEntity(const ObjectEntity&);
            bool moveDownObjectEntity(const ObjectEntity&);

            void createDefaultBody(const ObjectEntity&);
            void changeBodyShape(const ObjectEntity&, CollisionShape3D::ShapeType);
            void moveObjectInFrontOfCamera(const ObjectEntity&, bool);
            void removeBody(const ObjectEntity&);

            void changeLightType(const ObjectEntity&, Light::LightType);

            const ObjectEntity& updateObjectTransform(const ObjectEntity&, const Transform<float>&);
            const ObjectEntity& updateObjectProperties(const ObjectEntity&, Model::ShadowBehavior, Model::CullBehavior);
            const ObjectEntity& updateObjectTags(const ObjectEntity&, std::string_view);
            const ObjectEntity& updateObjectPhysicsProperties(const ObjectEntity&, float, float, float, float, float, float,
                    const Vector3<float>&, const Vector3<float>&);
            const ObjectEntity& updateObjectPhysicsShape(const ObjectEntity&, std::unique_ptr<const CollisionShape3D>);

        private:
            ObjectEntity& findObjectEntity(const ObjectEntity&) const;
    };

}
