#include <stdexcept>

#include "SoundController.h"
#include "scene/controller/sounds/soundshape/support/DefaultSoundShapeCreator.h"

namespace urchin
{

	SoundController::SoundController(MapHandler *mapHandler) :
			bIsModified(false),
			mapHandler(mapHandler)
	{

	}

	bool SoundController::isModified() const
	{
		return bIsModified;
	}

	void SoundController::markModified()
	{
		bIsModified = true;
	}

	void SoundController::resetModified()
	{
		bIsModified = false;
	}

	std::list<const SceneSound *> SoundController::getSceneSounds() const
	{
		const std::list<SceneSound *> &sceneSounds = mapHandler->getMap()->getSceneSounds();
		std::list<const SceneSound *> constSceneSounds;
		constSceneSounds.insert(constSceneSounds.begin(), sceneSounds.begin(), sceneSounds.end());

		return constSceneSounds;
	}

	void SoundController::addSceneSound(SceneSound *sceneSound)
	{
		mapHandler->getMap()->addSceneSound(sceneSound);

		markModified();
	}

	void SoundController::removeSceneSound(const SceneSound *constSceneSound)
	{
		SceneSound *sceneSound = findSceneSound(constSceneSound);
		mapHandler->getMap()->removeSceneSound(sceneSound);

		markModified();
	}

	void SoundController::changeSoundTrigger(const SceneSound *constSceneSound, SoundTrigger::TriggerType triggerType)
	{
		SceneSound *sceneSound = findSceneSound(constSceneSound);
		SoundTrigger *soundTrigger = sceneSound->getSoundTrigger();

		SoundTrigger *newSoundTrigger;
		if(triggerType == SoundTrigger::MANUAL_TRIGGER)
		{
			newSoundTrigger = new ManualTrigger(soundTrigger->getSoundBehavior());
		}else if(triggerType == SoundTrigger::SHAPE_TRIGGER)
		{
			const SoundShape *newDefaultShape = DefaultSoundShapeCreator(constSceneSound).createDefaultSoundShape(SoundShape::SPHERE_SHAPE);
			newSoundTrigger = new ShapeTrigger(soundTrigger->getSoundBehavior(), newDefaultShape);
		}else
		{
			throw std::invalid_argument("Impossible to change of trigger type: " + triggerType);
		}

		sceneSound->changeSoundTrigger(newSoundTrigger);

		markModified();
	}

	void SoundController::changeSoundShape(const SceneSound *constSceneSound, SoundShape::ShapeType shapeType)
	{
		SceneSound *sceneSound = findSceneSound(constSceneSound);
		SoundTrigger *soundTrigger = sceneSound->getSoundTrigger();

		const SoundShape *newShape = DefaultSoundShapeCreator(constSceneSound).createDefaultSoundShape(shapeType);
		SoundTrigger *newSoundTrigger = new ShapeTrigger(soundTrigger->getSoundBehavior(), newShape);

		sceneSound->changeSoundTrigger(newSoundTrigger);

		markModified();
	}

	const SceneSound *SoundController::updateSceneSoundGeneralProperties(const SceneSound *constSceneSound, float volume)
	{
		SceneSound *sceneSound = findSceneSound(constSceneSound);

		sceneSound->getSound()->setVolume(volume);

		markModified();
		return sceneSound;
	}

	const SceneSound *SoundController::updateScenePointSoundProperties(const SceneSound *constSceneSound, const Point3<float> &position,
			float inaudibleDistance)
	{
		SceneSound *sceneSound = findSceneSound(constSceneSound);
		auto *pointSound = dynamic_cast<PointSound *>(sceneSound->getSound());

		pointSound->setPosition(position);
		pointSound->setInaudibleDistance(inaudibleDistance);

		markModified();
		return sceneSound;
	}

	const SceneSound *SoundController::updateSceneSoundBehaviorProperties(const SceneSound *constSceneSound, SoundBehavior::PlayBehavior playBehavior,
			SoundBehavior::StopBehavior stopBehavior, float volumeDecreasePercentageOnStop)
	{
		SceneSound *sceneSound = findSceneSound(constSceneSound);
		SoundTrigger *soundTrigger = sceneSound->getSoundTrigger();

		SoundBehavior newSoundBehavior(playBehavior, stopBehavior, volumeDecreasePercentageOnStop);
		SoundTrigger *newSoundTrigger;

		if(soundTrigger->getTriggerType() == SoundTrigger::MANUAL_TRIGGER)
		{
			newSoundTrigger = new ManualTrigger(newSoundBehavior);
		}else if(soundTrigger->getTriggerType() == SoundTrigger::SHAPE_TRIGGER)
		{
			auto *shapeTrigger = dynamic_cast<ShapeTrigger *>(soundTrigger);
			SoundShape *clonedShape = shapeTrigger->getSoundShape()->clone();
			newSoundTrigger = new ShapeTrigger(newSoundBehavior, clonedShape);
		}else
		{
			throw std::invalid_argument("Impossible to update sound behavior because unknown trigger type: " + soundTrigger->getTriggerType());
		}

		sceneSound->changeSoundTrigger(newSoundTrigger);

		markModified();
		return sceneSound;
	}

	const SceneSound *SoundController::updateSceneSoundShape(const SceneSound *constSceneSound, const SoundShape *newSoundShape)
	{
		SceneSound *sceneSound = findSceneSound(constSceneSound);
		auto *shapeTrigger = dynamic_cast<ShapeTrigger *>(sceneSound->getSoundTrigger());

		auto *newShapeTrigger = new ShapeTrigger(shapeTrigger->getSoundBehavior(), newSoundShape);
		sceneSound->changeSoundTrigger(newShapeTrigger);

		markModified();
		return sceneSound;
	}

	SceneSound *SoundController::findSceneSound(const SceneSound *constSceneSound)
	{
		const std::list<SceneSound *> &sceneSounds = mapHandler->getMap()->getSceneSounds();
		auto it = std::find(sceneSounds.begin(), sceneSounds.end(), constSceneSound);

		if(it!=sceneSounds.end())
		{
			return *it;
		}

		throw std::invalid_argument("Impossible to find scene sound: " + constSceneSound->getName());
	}

}
