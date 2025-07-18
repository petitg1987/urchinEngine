#include <stdexcept>
#include <memory>
#include <utility>

#include <controller/object/ObjectController.h>
#include <controller/EntityControllerUtil.h>

namespace urchin {

    std::list<const ObjectEntity*> ObjectController::getObjectEntities() const {
        const auto& objectEntities = getMap().getObjectEntities();
        std::list<const ObjectEntity*> constObjectEntities;
        for (auto& objectEntity : objectEntities) {
            constObjectEntities.emplace_back(objectEntity.get());
        }

        return constObjectEntities;
    }

    const ObjectEntity* ObjectController::findObjectEntityByBodyId(std::string_view bodyId) const {
        for (const auto* objectEntity : getObjectEntities()) {
            if (objectEntity->getRigidBody() && objectEntity->getRigidBody()->getId() == bodyId) {
                return objectEntity;
            }
        }

        return nullptr;
    }

    std::pair<ObjectEntity*, std::size_t> ObjectController::addObjectEntity(std::unique_ptr<ObjectEntity> objectEntity) {
        std::pair<ObjectEntity*, std::size_t> addedObjectEntity = getMap().addObjectEntity(std::move(objectEntity));

        markModified();
        return addedObjectEntity;
    }

    void ObjectController::removeObjectEntity(const ObjectEntity& constObjectEntity) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        getMap().removeObjectEntity(objectEntity);

        markModified();
    }

    std::pair<ObjectEntity*, std::size_t> ObjectController::cloneObjectEntity(std::string newObjectName, const ObjectEntity& toCloneObjectEntity) {
        return addObjectEntity(toCloneObjectEntity.clone(std::move(newObjectName)));
    }

    void ObjectController::renameObjectEntity(const ObjectEntity& constObjectEntity, std::string newObjectName) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        objectEntity.setName(std::move(newObjectName));

        markModified();
    }

    void ObjectController::createDefaultBody(const ObjectEntity& constObjectEntity) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        auto rigidBody = DefaultRigidBodyGenerator(constObjectEntity).generate();
        objectEntity.setupInteractiveBody(std::move(rigidBody));

        markModified();
    }

    void ObjectController::changeBodyShape(const ObjectEntity& constObjectEntity, const DefaultShapeQuality& defaultShapeQuality, CollisionShape3D::ShapeType shapeType) {
        std::unique_ptr<const CollisionShape3D> newCollisionShape = DefaultBodyShapeGenerator(constObjectEntity, defaultShapeQuality).generate(shapeType);
        updateObjectPhysicsShape(constObjectEntity, std::move(newCollisionShape));
    }

    void ObjectController::moveObjectInFrontOfCamera(const ObjectEntity& constObjectEntity, bool isClonedEntity) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);

        Point3<float> currentPosition = objectEntity.getModel()->getTransform().getPosition();
        Point3<float> newPosition = EntityControllerUtil::determineNewPosition(currentPosition, isClonedEntity, getMap().getRenderer3d()->getCamera());
        objectEntity.updatePosition(newPosition);

        markModified();
    }

    void ObjectController::removeBody(const ObjectEntity& constObjectEntity) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        objectEntity.setupInteractiveBody(nullptr);

        markModified();
    }

    const ObjectEntity& ObjectController::updateObjectTransform(const ObjectEntity& constObjectEntity, const Transform<float>& transform) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);

        objectEntity.updateTransform(transform);

        markModified();
        return objectEntity;
    }

    const ObjectEntity& ObjectController::updateObjectProperties(const ObjectEntity& constObjectEntity, Model::ShadowBehavior shadowBehavior, Model::CullBehavior cullBehavior) {
        const ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        Model* model = objectEntity.getModel();

        model->setShadowBehavior(shadowBehavior);
        model->setCullBehavior(cullBehavior);

        markModified();
        return objectEntity;
    }

    const ObjectEntity& ObjectController::updateObjectPhysicsProperties(const ObjectEntity& constObjectEntity, float mass, float restitution, float friction, float rollingFriction,
            float linearDamping, float angularDamping, const Vector3<float>& linearFactor, const Vector3<float>& angularFactor) {
        const ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        RigidBody* rigidBody = objectEntity.getRigidBody();

        rigidBody->setMass(mass);
        rigidBody->setRestitution(restitution);
        rigidBody->setFriction(friction);
        rigidBody->setRollingFriction(rollingFriction);

        rigidBody->setDamping(linearDamping, angularDamping);

        rigidBody->setLinearFactor(linearFactor);
        rigidBody->setAngularFactor(angularFactor);

        markModified();
        return objectEntity;
    }

    const ObjectEntity& ObjectController::updateObjectPhysicsShape(const ObjectEntity& constObjectEntity, std::unique_ptr<const CollisionShape3D> newCollisionShape) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        objectEntity.updateObjectPhysicsShape(std::move(newCollisionShape));

        markModified();
        return objectEntity;
    }

    void ObjectController::changeLightType(const ObjectEntity& constObjectEntity, std::optional<Light::LightType> lightType) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);

        if (lightType.has_value()) {
            Point3<float> lightPosition = objectEntity.getModel()->getTransform().getPosition();

            if (lightType.value() == Light::LightType::SUN) {
                objectEntity.setLight(std::make_shared<SunLight>(Vector3(1.0f, -1.0f, 0.0f)));
            } else if (lightType.value() == Light::LightType::OMNIDIRECTIONAL) {
                objectEntity.setLight(std::make_shared<OmnidirectionalLight>(lightPosition));
            } else if (lightType.value() == Light::LightType::SPOT) {
                objectEntity.setLight(std::make_shared<SpotLight>(lightPosition, Vector3(0.0f, -1.0f, 0.0f), 30.0f, 45.0f));
            } else {
                throw std::invalid_argument("Unknown the light type to create a new light: " + std::to_string((int)lightType.value()));
            }
        } else {
            objectEntity.setLight(std::shared_ptr<Light>(nullptr));
        }

        markModified();
    }

    const ObjectEntity& ObjectController::updateLightGeneralProperties(const ObjectEntity& constObjectEntity, const Point3<float>& lightColor, bool enablePbr, bool isProduceShadow) {
        const ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        Light* light = objectEntity.getLight();

        light->setLightColor(lightColor);
        light->setPbrEnabled(enablePbr);
        if (light->isProduceShadow() != isProduceShadow) {
            light->setProduceShadow(isProduceShadow);
        }

        markModified();
        return objectEntity;
    }

    const ObjectEntity& ObjectController::updateSunLightProperties(const ObjectEntity& constObjectEntity, const Vector3<float>& direction) {
        const ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        auto* sunLight = static_cast<SunLight*>(objectEntity.getLight());

        sunLight->setDirection(direction);

        markModified();
        return objectEntity;
    }

    const ObjectEntity& ObjectController::updateOmnidirectionalLightProperties(const ObjectEntity& constObjectEntity, float attenuation, const Point3<float>& position) {
        const ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        auto* omnidirectionalLight = static_cast<OmnidirectionalLight*>(objectEntity.getLight());

        omnidirectionalLight->setAttenuation(attenuation);
        omnidirectionalLight->setPosition(position);

        markModified();
        return objectEntity;
    }

    const ObjectEntity& ObjectController::updateSpotLightProperties(const ObjectEntity& constObjectEntity, float attenuation, const Point3<float>& position, const Vector3<float>& direction,
                                                                    float innerAngleInDegrees, float outerAngleInDegrees) {
        const ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);
        auto* spotLight = static_cast<SpotLight*>(objectEntity.getLight());

        spotLight->setAttenuation(attenuation);
        spotLight->setPosition(position);
        spotLight->setDirection(direction);
        spotLight->setAngles(innerAngleInDegrees, outerAngleInDegrees);

        markModified();
        return objectEntity;
    }

    void ObjectController::changeSound(const ObjectEntity& constObjectEntity, std::optional<Sound::SoundType> soundType, std::string soundFilename) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);

        if (soundType.has_value()) {
            std::shared_ptr<Sound> sound;
            if (soundType.value() == Sound::SoundType::LOCALIZABLE) {
                Point3<float> soundPosition = objectEntity.getModel()->getTransform().getPosition();
                sound = std::make_shared<LocalizableSound>(std::move(soundFilename), Sound::SoundCategory::EFFECTS, 1.0f, soundPosition, 5.0f);
            } else if (soundType.value() == Sound::SoundType::GLOBAL) {
                sound = std::make_shared<GlobalSound>(std::move(soundFilename), Sound::SoundCategory::MUSIC, 1.0f);
            } else {
                throw std::invalid_argument("Unknown the sound type to create a new sound: " + std::to_string((int)soundType.value()));
            }

            auto defaultSoundTrigger = std::make_shared<AutoTrigger>(PlayBehavior::PLAY_ONCE, sound);
            objectEntity.setSoundComponent(std::make_shared<SoundComponent>(sound, defaultSoundTrigger));
        } else {
            objectEntity.setSoundComponent(std::shared_ptr<SoundComponent>(nullptr));
        }

        markModified();
    }

    void ObjectController::updateSoundComponent(const ObjectEntity& constObjectEntity, std::shared_ptr<Sound> sound, std::shared_ptr<SoundTrigger> soundTrigger) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);

        objectEntity.setSoundComponent(std::make_shared<SoundComponent>(std::move(sound), std::move(soundTrigger)));

        markModified();
    }

    const ObjectEntity& ObjectController::updateObjectTags(const ObjectEntity& constObjectEntity, std::string_view tagsValues) {
        ObjectEntity& objectEntity = findObjectEntity(constObjectEntity);

        objectEntity.removeAllTags();
        std::vector<std::string> tagsList = StringUtil::split(tagsValues, TAGS_SEPARATOR);
        for (const std::string& tag: tagsList) {
            objectEntity.addTag(tag);
        }

        markModified();
        return objectEntity;
    }

    ObjectEntity& ObjectController::findObjectEntity(const ObjectEntity& constObjectEntity) const {
        const auto& objectEntities = getMap().getObjectEntities();
        auto it = std::ranges::find_if(objectEntities, [&constObjectEntity](const auto& o){return o.get() == &constObjectEntity;});

        if (it != objectEntities.end()) {
            return *(*it);
        }

        throw std::invalid_argument("Impossible to find object entity: " + constObjectEntity.getName());
    }

}
