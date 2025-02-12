#include <AIEnvironment.h>

#include <model/ObjectEntity.h>
#include <model/builder/AIEntityBuilder.h>

namespace urchin {

    ObjectEntity::ObjectEntity() :
            renderer3d(nullptr),
            physicsWorld(nullptr),
            soundEnvironment(nullptr),
            aiEnvironment(nullptr),
            rigidBody(nullptr),
            aiObject(nullptr) {

    }

    ObjectEntity::~ObjectEntity() {
        if (soundEnvironment) {
            soundEnvironment->removeSoundComponent(soundComponent.get());
        }
        if (renderer3d) {
            renderer3d->getLightManager().removeLight(light.get());
            renderer3d->removeModel(model.get());
        }
        deleteRigidBody();
        deleteAIObjects();
    }

    void ObjectEntity::setup(Renderer3d* renderer3d, PhysicsWorld* physicsWorld, SoundEnvironment* soundEnvironment, AIEnvironment* aiEnvironment) {
        this->renderer3d = renderer3d;
        this->physicsWorld = physicsWorld;
        this->soundEnvironment = soundEnvironment;
        this->aiEnvironment = aiEnvironment;

        if (renderer3d) {
            renderer3d->addModel(model);
            renderer3d->getLightManager().addLight(light);
        }
        if (physicsWorld && rigidBody) {
            physicsWorld->addBody(rigidBody);
        }
        if (soundEnvironment && soundComponent) {
            soundEnvironment->addSoundComponent(soundComponent);
        }
        if (aiEnvironment && aiObject) {
            aiEnvironment->addEntity(aiObject);
        }
    }

    void ObjectEntity::setName(std::string name) {
        this->name = std::move(name);
    }

    const std::string& ObjectEntity::getName() const {
        return name;
    }

    void ObjectEntity::setModel(const std::shared_ptr<Model>& model) {
        if (renderer3d) {
            renderer3d->removeModel(this->model.get());
            renderer3d->addModel(model);
        }
        this->model = model;
    }

    Model* ObjectEntity::getModel() const {
        return model.get();
    }

    const std::shared_ptr<Model>& ObjectEntity::getModelPtr() const {
        return model;
    }

    void ObjectEntity::setupInteractiveBody(std::shared_ptr<RigidBody> rigidBody) {
        setupRigidBody(std::move(rigidBody));
        setupAIObject();
    }

    void ObjectEntity::updateObjectPhysicsShape(std::unique_ptr<const CollisionShape3D> newCollisionShape) {
        PhysicsTransform physicsTransform(getModel()->getTransform().getPosition(), getModel()->getTransform().getOrientation());

        auto newRigidBody = std::make_unique<RigidBody>(getName(), physicsTransform, std::move(newCollisionShape));
        newRigidBody->setMass(getRigidBody()->getMass());
        newRigidBody->setRestitution(getRigidBody()->getRestitution());
        newRigidBody->setFriction(getRigidBody()->getFriction());
        newRigidBody->setRollingFriction(getRigidBody()->getRollingFriction());
        newRigidBody->setDamping(getRigidBody()->getLinearDamping(), getRigidBody()->getAngularDamping());
        newRigidBody->setLinearFactor(getRigidBody()->getLinearFactor());
        newRigidBody->setAngularFactor(getRigidBody()->getAngularFactor());

        setupInteractiveBody(std::move(newRigidBody));
    }

    RigidBody* ObjectEntity::getRigidBody() const {
        return rigidBody.get();
    }

    const std::shared_ptr<RigidBody>& ObjectEntity::getRigidBodyPtr() const {
        return rigidBody;
    }

    void ObjectEntity::setupRigidBody(std::shared_ptr<RigidBody> rigidBody) {
        deleteRigidBody();

        this->rigidBody = std::move(rigidBody);
        if (physicsWorld && this->rigidBody) {
            physicsWorld->addBody(this->rigidBody);
        }
    }

    Light* ObjectEntity::getLight() const {
        return light.get();
    }

    std::shared_ptr<Light>& ObjectEntity::getLightPtr() {
        return light;
    }

    void ObjectEntity::setLight(std::shared_ptr<Light> light) {
        if (renderer3d) {
            renderer3d->getLightManager().removeLight(this->light.get());
            renderer3d->getLightManager().addLight(light);
        }

        this->light = std::move(light);
    }

    SoundComponent* ObjectEntity::getSoundComponent() const {
        return soundComponent.get();
    }

    void ObjectEntity::setSoundComponent(const std::shared_ptr<SoundComponent>& soundComponent) {
        if (soundEnvironment) {
            soundEnvironment->removeSoundComponent(this->soundComponent.get());
            soundEnvironment->addSoundComponent(soundComponent);
        }

        this->soundComponent = soundComponent;
    }

    void ObjectEntity::setupAIObject() {
        deleteAIObjects();

        if (!rigidBody || !model->getConstMeshes()) {
            this->aiObject = nullptr;
        } else {
            std::string aiObjectName = "#" + rigidBody->getId(); //prefix to avoid collision name with terrains
            this->aiObject = AIEntityBuilder::buildAIObject(aiObjectName, rigidBody->getShape(), rigidBody->getTransform().toTransform());
            if (aiEnvironment) {
                aiEnvironment->addEntity(aiObject);
            }
        }
    }

    void ObjectEntity::deleteRigidBody() {
        if (physicsWorld && rigidBody) {
            physicsWorld->removeBody(*rigidBody);
        }
        rigidBody = nullptr;
    }

    void ObjectEntity::deleteAIObjects() const {
        if (aiEnvironment && aiObject) {
            aiEnvironment->removeEntity(aiObject);
        }
    }

    std::unique_ptr<ObjectEntity> ObjectEntity::clone(std::string newName) const {
        auto newObject = std::make_unique<ObjectEntity>();
        newObject->setName(std::move(newName));
        newObject->setModel(std::make_unique<Model>(*getModel()));

        RigidBody* toCloneRigidBody = getRigidBody();
        if (toCloneRigidBody) {
            auto rigidBody = std::make_unique<RigidBody>(*toCloneRigidBody);
            rigidBody->setId(newObject->getName());
            newObject->setupInteractiveBody(std::move(rigidBody));
        }

        SoundComponent* toCloneSoundComponent = getSoundComponent();
        if (toCloneSoundComponent) {
            newObject->setSoundComponent(std::make_unique<SoundComponent>(*toCloneSoundComponent));
        }

        Light* toCloneLight = getLight();
        if (toCloneLight) {
            newObject->setLight(toCloneLight->clone());
        }

        newObject->addTags(getTags());
        return newObject;
    }

    void ObjectEntity::updateTransform(const Transform<float>& transform) {
        Transform<float> oldTransform = model->getTransform();
        Vector3<float> positionDelta = oldTransform.getPosition().vector(transform.getPosition());

        model->setTransform(transform);
        if (rigidBody) {
            rigidBody->setTransform(PhysicsTransform(model->getTransform().getPosition(), model->getTransform().getOrientation()));

            Vector3<float> scaleRatio = transform.getScale() / oldTransform.getScale();
            auto scaledCollisionShape = rigidBody->getShape().scale(scaleRatio);
            updateObjectPhysicsShape(std::move(scaledCollisionShape));
        }

        if (light) {
            if (light->getLightType() == Light::LightType::OMNIDIRECTIONAL) {
                auto* omnidirectionalLight = dynamic_cast<OmnidirectionalLight*>(light.get());
                omnidirectionalLight->setPosition(omnidirectionalLight->getPosition().translate(positionDelta));
            } else if (light->getLightType() == Light::LightType::SPOT) {
                auto* spotLight = dynamic_cast<SpotLight*>(light.get());
                spotLight->setPosition(spotLight->getPosition().translate(positionDelta));
            }
        }

        if (soundComponent) {
            Sound& sound = soundComponent->getSound();
            if (sound.getSoundType() == Sound::SoundType::LOCALIZABLE) {
                auto* localizableSound = dynamic_cast<LocalizableSound*>(&sound);
                localizableSound->setPosition(localizableSound->getPosition().translate(positionDelta));
            }

            if (soundComponent->getSoundTrigger().getTriggerType() == SoundTrigger::TriggerType::AREA_TRIGGER) {
                SoundShape& soundShape = soundComponent->getAreaTrigger().getSoundShape();
                soundShape.updateCenterPosition(soundShape.getCenterPosition().translate(positionDelta));
            }
        }
    }

    void ObjectEntity::updatePosition(const Point3<float>& newPosition) {
        updateTransform(Transform(newPosition, model->getTransform().getOrientation(), model->getTransform().getScale()));
    }

    void ObjectEntity::updateOrientation(const Quaternion<float>& newOrientation) {
        updateTransform(Transform(model->getTransform().getPosition(), newOrientation, model->getTransform().getScale()));
    }

    void ObjectEntity::refresh() const {
        if (rigidBody && rigidBody->isActive()) {
            PhysicsTransform physicsTransform = rigidBody->getTransform();
            model->setTransform(Transform(physicsTransform.getPosition(), physicsTransform.getOrientation(), model->getTransform().getScale()));
            if (aiObject) {
                aiObject->updateTransform(physicsTransform.getPosition(), physicsTransform.getOrientation());
            }
        }
    }

}
