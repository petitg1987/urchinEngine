#pragma once

#include <list>
#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAggregation.h>

#include "controller/AbstractController.h"

namespace urchin {

    class ObjectController final : public AbstractController {
        public:
            static constexpr char TAGS_SEPARATOR = ',';

            std::list<const ObjectEntity*> getObjectEntities() const;
            const ObjectEntity* findObjectEntityByBodyId(std::string_view) const;
            ObjectEntity& addObjectEntity(std::unique_ptr<ObjectEntity>);
            void removeObjectEntity(const ObjectEntity&);
            ObjectEntity& cloneObjectEntity(std::string, const ObjectEntity&);
            void updateObjectEntity(const ObjectEntity&, std::string, std::vector<std::string>);
            std::map<const ObjectEntity*, std::vector<std::string>> retrieveObjectEntitiesToUpdate(const std::vector<std::string>&, const std::vector<std::string>&) const;
            std::vector<const ObjectEntity*> updateObjectEntities(const std::vector<std::string>&, const std::vector<std::string>&);

            void moveObjectInFrontOfCamera(const ObjectEntity&, bool);
            void changeMeshesFile(const ObjectEntity&, const std::string&);
            const ObjectEntity& updateObjectTransform(const ObjectEntity&, const Transform<float>&);
            const ObjectEntity& updateObjectProperties(const ObjectEntity&, Model::ShadowBehavior, uint8_t, Model::CullBehavior);

            void createDefaultBody(const ObjectEntity&);
            void changeBodyShape(const ObjectEntity&, const DefaultShapeQuality&, CollisionShape3D::ShapeType);
            void removeBody(const ObjectEntity&);
            const ObjectEntity& updateObjectPhysicsProperties(const ObjectEntity&, float, float, float, float, float, float, const Vector3<float>&, const Vector3<float>&);
            const ObjectEntity& updateObjectPhysicsShape(const ObjectEntity&, std::unique_ptr<const CollisionShape3D>);

            void changeLightType(const ObjectEntity&, std::optional<Light::LightType>);
            const ObjectEntity& updateLightGeneralProperties(const ObjectEntity&, const Point3<float>&, bool, uint8_t, bool, float);
            const ObjectEntity& updateSunLightProperties(const ObjectEntity&, const Vector3<float>&);
            const ObjectEntity& updateOmnidirectionalLightProperties(const ObjectEntity&, float, float, const Point3<float>&);
            const ObjectEntity& updateSpotLightProperties(const ObjectEntity&, float, float, const Point3<float>&, const Vector3<float>&, float, float);

            void changeSound(const ObjectEntity&, std::optional<Sound::SoundType>, std::string);
            void updateSoundComponent(const ObjectEntity&, std::shared_ptr<Sound>, std::shared_ptr<SoundTrigger>);

            const ObjectEntity& updateObjectTags(const ObjectEntity&, std::string_view);

        private:
            ObjectEntity& findObjectEntity(const ObjectEntity&) const;
    };

}
