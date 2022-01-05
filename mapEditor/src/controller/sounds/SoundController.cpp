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
        SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        ZoneTrigger& zoneTrigger = soundEntity.getSoundComponent()->getZoneTrigger();

        auto newShape = DefaultSoundShapeCreator(constSoundEntity.getSoundComponent()->getSound()).createDefaultSoundShape(shapeType);
        zoneTrigger.setSoundShape(std::move(newShape));

        markModified();
    }

    const SoundEntity& SoundController::updateSpatialSoundProperties(const SoundEntity& constSoundEntity, const Point3<float>& position,
            float inaudibleDistance) {
        const SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        auto& pointSound = static_cast<SpatialSound&>(soundEntity.getSoundComponent()->getSound());

        pointSound.setPosition(position);
        pointSound.setInaudibleDistance(inaudibleDistance);

        markModified();
        return soundEntity;
    }

    const SoundEntity& SoundController::updateSoundTriggerGeneralProperties(const SoundEntity& constSoundEntity, PlayBehavior playBehavior) {
        SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        SoundTrigger& soundTrigger = soundEntity.getSoundComponent()->getSoundTrigger();

        soundTrigger.setPlayBehavior(playBehavior);

        markModified();
        return soundEntity;
    }

    const SoundEntity& SoundController::updateSoundShape(const SoundEntity& constSoundEntity, std::unique_ptr<const SoundShape> newSoundShape) {
        SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
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
