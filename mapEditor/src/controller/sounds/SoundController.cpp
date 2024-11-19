#include <stdexcept>

#include <controller/sounds/SoundController.h>
#include <panel/sounds/soundshape/support/DefaultSoundShapeCreator.h>
#include <controller/EntityControllerUtil.h>

namespace urchin {

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
        AreaTrigger& areaTrigger = soundEntity.getSoundComponent()->getAreaTrigger();

        auto newShape = DefaultSoundShapeCreator(constSoundEntity.getSoundComponent()->getSound()).createDefaultSoundShape(shapeType);
        areaTrigger.setSoundShape(std::move(newShape));

        markModified();
    }

    void SoundController::moveSoundInFrontOfCamera(const SoundEntity& constSoundEntity, bool isClonedEntity) {
        if (constSoundEntity.getSoundComponent()->getSound().getSoundType() == Sound::SoundType::LOCALIZABLE) {
            auto& localizableSound = static_cast<LocalizableSound&>(constSoundEntity.getSoundComponent()->getSound());
            Point3<float> currentPosition = localizableSound.getPosition();
            Point3<float> newPosition = EntityControllerUtil::determineNewPosition(currentPosition, isClonedEntity, getMap().getRenderer3d()->getCamera());
            localizableSound.setPosition(newPosition);

            markModified();
        }

        if (constSoundEntity.getSoundComponent()->getSoundTrigger().getTriggerType() == SoundTrigger::TriggerType::AREA_TRIGGER) {
            auto& areaTrigger = static_cast<AreaTrigger&>(constSoundEntity.getSoundComponent()->getSoundTrigger());
            Point3<float> currentPosition = areaTrigger.getSoundShape().getCenterPosition();
            Point3<float> newPosition = EntityControllerUtil::determineNewPosition(currentPosition, isClonedEntity, getMap().getRenderer3d()->getCamera());
            areaTrigger.getSoundShape().updateCenterPosition(newPosition);

            markModified();
        }
    }

    const SoundEntity& SoundController::updateLocalizableSoundProperties(const SoundEntity& constSoundEntity, const Point3<float>& position, float radius) {
        const SoundEntity& soundEntity = findSoundEntity(constSoundEntity);
        auto& localizableSound = static_cast<LocalizableSound&>(soundEntity.getSoundComponent()->getSound());

        localizableSound.setPosition(position);
        localizableSound.setRadius(radius);

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
        AreaTrigger& areaTrigger = soundEntity.getSoundComponent()->getAreaTrigger();

        areaTrigger.setSoundShape(std::move(newSoundShape));

        markModified();
        return soundEntity;
    }

    SoundEntity& SoundController::findSoundEntity(const SoundEntity& constSoundEntity) const {
        const auto& soundEntities = getMap().getSoundEntities();
        auto it = std::ranges::find_if(soundEntities, [&constSoundEntity](const auto& o){return o.get() == &constSoundEntity;});

        if (it != soundEntities.end()) {
            return *(*it);
        }

        throw std::invalid_argument("Impossible to find sound entity: " + constSoundEntity.getName());
    }

}
