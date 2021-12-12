#pragma once

#include <list>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinAggregation.h>

#include <controller/AbstractController.h>

namespace urchin {

    class ObjectController : public AbstractController {
        public:
            static constexpr char TAGS_SEPARATOR = ',';

            ObjectController();

            std::list<const ObjectEntity*> getObjectEntities() const;
            const ObjectEntity* findObjectEntityByBodyId(const std::string&) const;
            void addObjectEntity(std::unique_ptr<ObjectEntity>);
            void removeObjectEntity(const ObjectEntity&);
            void cloneObjectEntity(std::unique_ptr<ObjectEntity>, const ObjectEntity&);

            void createDefaultBody(const ObjectEntity&);
            void changeBodyShape(const ObjectEntity&, CollisionShape3D::ShapeType);
            void removeBody(const ObjectEntity&);

            const ObjectEntity& updateObjectTransform(const ObjectEntity&, const Transform<float>&);
            const ObjectEntity& updateObjectFlags(const ObjectEntity&, bool);
            const ObjectEntity& updateObjectTags(const ObjectEntity&, const std::string&);
            const ObjectEntity& updateObjectPhysicsProperties(const ObjectEntity&, float, float, float, float, float, float,
                    const Vector3<float>&, const Vector3<float>&);
            const ObjectEntity& updateObjectPhysicsShape(const ObjectEntity&, std::unique_ptr<const CollisionShape3D>);

        private:
            ObjectEntity& findObjectEntity(const ObjectEntity&);
    };

}
