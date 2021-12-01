#include <stdexcept>

#include <controller/sounds/SoundController.h>
#include <panel/sounds/soundshape/support/DefaultSoundShapeCreator.h>

namespace urchin {

    SoundController::SoundController() :
            AbstractController() {

    }

    std::list<const SceneSound*> SoundController::getSceneSounds() const {
        const auto& sceneSounds = getMapHandler()->getMap().getSceneSounds();
        std::list<const SceneSound*> constSceneSounds;
        for (auto& sceneSound : sceneSounds) {
            constSceneSounds.emplace_back(sceneSound.get());
        }

        return constSceneSounds;
    }

    void SoundController::addSceneSound(std::unique_ptr<SceneSound> sceneSound) {
        getMapHandler()->getMap().addSceneSound(std::move(sceneSound));

        markModified();
    }

    void SoundController::removeSceneSound(const SceneSound& constSceneSound) {
        SceneSound& sceneSound = findSceneSound(constSceneSound);
        getMapHandler()->getMap().removeSceneSound(sceneSound);

        markModified();
    }

    void SoundController::changeSoundTrigger(const SceneSound& constSceneSound, SoundTrigger::TriggerType triggerType) {
        SceneSound& sceneSound = findSceneSound(constSceneSound);
        const SoundTrigger* soundTrigger = sceneSound.getSoundTrigger();

        std::shared_ptr<SoundTrigger> newSoundTrigger;
        if (triggerType == SoundTrigger::MANUAL_TRIGGER) {
            newSoundTrigger = std::make_shared<ManualTrigger>(soundTrigger->getPlayBehavior());
        } else if (triggerType == SoundTrigger::SHAPE_TRIGGER) {
            auto newDefaultShape = DefaultSoundShapeCreator(constSceneSound).createDefaultSoundShape(SoundShape::SPHERE_SHAPE);
            newSoundTrigger = std::make_shared<ShapeTrigger>(soundTrigger->getPlayBehavior(), std::move(newDefaultShape));
        } else {
            throw std::invalid_argument("Impossible to change of trigger type: " + std::to_string(triggerType));
        }

        sceneSound.changeSoundTrigger(newSoundTrigger);

        markModified();
    }

    void SoundController::changeSoundShape(const SceneSound& constSceneSound, SoundShape::ShapeType shapeType) {
        SceneSound& sceneSound = findSceneSound(constSceneSound);
        const SoundTrigger* soundTrigger = sceneSound.getSoundTrigger();

        auto newShape = DefaultSoundShapeCreator(constSceneSound).createDefaultSoundShape(shapeType);
        auto newSoundTrigger = std::make_shared<ShapeTrigger>(soundTrigger->getPlayBehavior(), std::move(newShape));

        sceneSound.changeSoundTrigger(newSoundTrigger);

        markModified();
    }

    const SceneSound& SoundController::updateSceneSpatialSoundProperties(const SceneSound& constSceneSound, const Point3<float>& position,
            float inaudibleDistance) {
        const SceneSound& sceneSound = findSceneSound(constSceneSound);
        auto* pointSound = static_cast<SpatialSound*>(sceneSound.getSound());

        pointSound->setPosition(position);
        pointSound->setInaudibleDistance(inaudibleDistance);

        markModified();
        return sceneSound;
    }

    const SceneSound& SoundController::updateSceneSoundTriggerGeneralProperties(const SceneSound& constSceneSound, SoundTrigger::PlayBehavior playBehavior) {
        SceneSound& sceneSound = findSceneSound(constSceneSound);
        SoundTrigger* soundTrigger = sceneSound.getSoundTrigger();

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

        sceneSound.changeSoundTrigger(newSoundTrigger);

        markModified();
        return sceneSound;
    }

    const SceneSound& SoundController::updateSceneSoundShape(const SceneSound& constSceneSound, std::unique_ptr<const SoundShape> newSoundShape) {
        SceneSound& sceneSound = findSceneSound(constSceneSound);
        const auto* shapeTrigger = static_cast<ShapeTrigger*>(sceneSound.getSoundTrigger());

        auto newShapeTrigger = std::make_shared<ShapeTrigger>(shapeTrigger->getPlayBehavior(), std::move(newSoundShape));
        sceneSound.changeSoundTrigger(newShapeTrigger);

        markModified();
        return sceneSound;
    }

    SceneSound& SoundController::findSceneSound(const SceneSound& constSceneSound) {
        const auto& sceneSounds = getMapHandler()->getMap().getSceneSounds();
        auto it = std::ranges::find_if(sceneSounds, [&constSceneSound](const auto& o){return o.get() == &constSceneSound;});

        if (it != sceneSounds.end()) {
            return *(*it);
        }

        throw std::invalid_argument("Impossible to find scene sound: " + constSceneSound.getName());
    }

}
