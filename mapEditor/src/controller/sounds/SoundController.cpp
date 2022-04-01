#include <stdexcept>

#include <controller/sounds/SoundController.h>
#include <panel/sounds/soundshape/support/DefaultSoundShapeCreator.h>

namespace urchin {

    SoundController::SoundController() :
            AbstractController() {

    }

    std::list<const SoundEntity*> SoundController::getSoundEntities() const {
        const auto& soundEntities = getMap().getSoundEntities();
        std::list<const SoundEntity*> constSoundEntities;
        for (auto& soundEntity : soundEntities) {
            constSoundEntities.emplace_back(soundEntity.get());
        }

        return constSoundEntities;
    }

    void SoundController::addSoundEntity(std::unique_ptr<SoundEntity> soundEntity) {
        getMap().addSoundEntity(std::move(soundEntity));

        markModified();
    }

    void SoundController::removeSoundEntity(const SoundEntity& constSoundEntity) {
        SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        getMap().removeSoundEntity(soundEntity);

        markModified();
    }
    
    void SoundController::changeSoundShape(const SoundEntity& constSoundEntity, SoundShape::ShapeType shapeType) {
        const SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        ZoneTrigger& zoneTrigger = soundEntity.getSoundComponent()->getZoneTrigger();

        auto newShape = DefaultSoundShapeCreator(constSoundEntity.getSoundComponent()->getSound()).createDefaultSoundShape(shapeType);
        zoneTrigger.setSoundShape(std::move(newShape));

        markModified();
    }

    void SoundController::moveSoundInFrontOfCamera(const SoundEntity& constSoundEntity) {
        const Camera* camera = getMap().getRenderer3d()->getCamera();
        Point3<float> newPosition = camera->getPosition().translate(camera->getView() * 5.0f);

        if (constSoundEntity.getSoundComponent()->getSound().getSoundType() == Sound::SoundType::SPATIAL) {
            static_cast<SpatialSound&>(constSoundEntity.getSoundComponent()->getSound()).setPosition(newPosition);
            markModified();
        }

        if (constSoundEntity.getSoundComponent()->getSoundTrigger().getTriggerType() == SoundTrigger::TriggerType::ZONE_TRIGGER) {
            static_cast<ZoneTrigger&>(constSoundEntity.getSoundComponent()->getSoundTrigger()).getSoundShape().updatePosition(newPosition);
            markModified();
        }
    }

    const SoundEntity& SoundController::updateSpatialSoundProperties(const SoundEntity& constSoundEntity, const Point3<float>& position,
            float inaudibleDistance) {
        const SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        auto& spatialSound = static_cast<SpatialSound&>(soundEntity.getSoundComponent()->getSound());

        spatialSound.setPosition(position);
        spatialSound.setInaudibleDistance(inaudibleDistance);

        markModified();
        return soundEntity;
    }

    const SoundEntity& SoundController::updateSoundTriggerGeneralProperties(const SoundEntity& constSoundEntity, PlayBehavior playBehavior) {
        const SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        SoundTrigger& soundTrigger = soundEntity.getSoundComponent()->getSoundTrigger();

        soundTrigger.setPlayBehavior(playBehavior);

        markModified();
        return soundEntity;
    }

    const SoundEntity& SoundController::updateSoundShape(const SoundEntity& constSoundEntity, std::unique_ptr<SoundShape> newSoundShape) {
        const SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        ZoneTrigger& zoneTrigger = soundEntity.getSoundComponent()->getZoneTrigger();

        zoneTrigger.setSoundShape(std::move(newSoundShape));

        markModified();
        return soundEntity;
    }

    SoundEntity& SoundController::findSoundEntity(const SoundEntity& constSoundEntity) {
        const auto& soundEntities = getMap().getSoundEntities();
        auto it = std::ranges::find_if(soundEntities, [&constSoundEntity](const auto& o){return o.get() == &constSoundEntity;});

        if (it != soundEntities.end()) {
            return *(*it);
        }

        throw std::invalid_argument("Impossible to find sound entity: " + constSoundEntity.getName());
    }

}
