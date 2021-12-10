#include <stdexcept>

#include <controller/sounds/SoundController.h>
#include <panel/sounds/soundshape/support/DefaultSoundShapeCreator.h>

namespace urchin {

    SoundController::SoundController() :
            AbstractController() {

    }

    std::list<const SoundEntity*> SoundController::getSoundEntities() const {
        const auto& soundEntities = getMapHandler()->getMap().getSoundEntities();
        std::list<const SoundEntity*> constSoundEntities;
        for (auto& soundEntity : soundEntities) {
            constSoundEntities.emplace_back(soundEntity.get());
        }

        return constSoundEntities;
    }

    void SoundController::addSoundEntity(std::unique_ptr<SoundEntity> soundEntity) {
        getMapHandler()->getMap().addSoundEntity(std::move(soundEntity));

        markModified();
    }

    void SoundController::removeSoundEntity(const SoundEntity& constSoundEntity) {
        SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        getMapHandler()->getMap().removeSoundEntity(soundEntity);

        markModified();
    }

    void SoundController::changeSoundTrigger(const SoundEntity& constSoundEntity, SoundTrigger::TriggerType triggerType) {
        SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        const SoundTrigger* soundTrigger = soundEntity.getSoundTrigger();

        std::shared_ptr<SoundTrigger> newSoundTrigger;
        if (triggerType == SoundTrigger::MANUAL_TRIGGER) {
            newSoundTrigger = std::make_shared<ManualTrigger>(soundTrigger->getPlayBehavior());
        } else if (triggerType == SoundTrigger::SHAPE_TRIGGER) {
            auto newDefaultShape = DefaultSoundShapeCreator(constSoundEntity).createDefaultSoundShape(SoundShape::SPHERE_SHAPE);
            newSoundTrigger = std::make_shared<ShapeTrigger>(soundTrigger->getPlayBehavior(), std::move(newDefaultShape));
        } else {
            throw std::invalid_argument("Impossible to change of trigger type: " + std::to_string(triggerType));
        }

        soundEntity.changeSoundTrigger(newSoundTrigger);

        markModified();
    }

    void SoundController::changeSoundShape(const SoundEntity& constSoundEntity, SoundShape::ShapeType shapeType) {
        SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        const SoundTrigger* soundTrigger = soundEntity.getSoundTrigger();

        auto newShape = DefaultSoundShapeCreator(constSoundEntity).createDefaultSoundShape(shapeType);
        auto newSoundTrigger = std::make_shared<ShapeTrigger>(soundTrigger->getPlayBehavior(), std::move(newShape));

        soundEntity.changeSoundTrigger(newSoundTrigger);

        markModified();
    }

    const SoundEntity& SoundController::updateSpatialSoundProperties(const SoundEntity& constSoundEntity, const Point3<float>& position,
            float inaudibleDistance) {
        const SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        auto* pointSound = static_cast<SpatialSound*>(soundEntity.getSound());

        pointSound->setPosition(position);
        pointSound->setInaudibleDistance(inaudibleDistance);

        markModified();
        return soundEntity;
    }

    const SoundEntity& SoundController::updateSoundTriggerGeneralProperties(const SoundEntity& constSoundEntity, SoundTrigger::PlayBehavior playBehavior) {
        SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        SoundTrigger* soundTrigger = soundEntity.getSoundTrigger();

        std::shared_ptr<SoundTrigger> newSoundTrigger;
        if (soundTrigger->getTriggerType() == SoundTrigger::MANUAL_TRIGGER) {
            newSoundTrigger = std::make_shared<ManualTrigger>(playBehavior);
        } else if (soundTrigger->getTriggerType() == SoundTrigger::SHAPE_TRIGGER) {
            const auto* shapeTrigger = static_cast<ShapeTrigger*>(soundTrigger);
            auto clonedShape = shapeTrigger->getSoundShape().clone();
            newSoundTrigger = std::make_shared<ShapeTrigger>(playBehavior, std::move(clonedShape));
        } else {
            throw std::invalid_argument("Impossible to update sound trigger because unknown trigger type: " + std::to_string(soundTrigger->getTriggerType()));
        }

        soundEntity.changeSoundTrigger(newSoundTrigger);

        markModified();
        return soundEntity;
    }

    const SoundEntity& SoundController::updateSoundShape(const SoundEntity& constSoundEntity, std::unique_ptr<const SoundShape> newSoundShape) {
        SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        const auto* shapeTrigger = static_cast<ShapeTrigger*>(soundEntity.getSoundTrigger());

        auto newShapeTrigger = std::make_shared<ShapeTrigger>(shapeTrigger->getPlayBehavior(), std::move(newSoundShape));
        soundEntity.changeSoundTrigger(newShapeTrigger);

        markModified();
        return soundEntity;
    }

    SoundEntity& SoundController::findSoundEntity(const SoundEntity& constSoundEntity) {
        const auto& soundEntities = getMapHandler()->getMap().getSoundEntities();
        auto it = std::ranges::find_if(soundEntities, [&constSoundEntity](const auto& o){return o.get() == &constSoundEntity;});

        if (it != soundEntities.end()) {
            return *(*it);
        }

        throw std::invalid_argument("Impossible to find sound entity: " + constSoundEntity.getName());
    }

}
