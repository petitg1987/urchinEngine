#include "trigger/ShapeTrigger.h"

namespace urchin
{

	/**
	 * @param soundShape Delimited shape which trigger the sound play
	 */
	ShapeTrigger::ShapeTrigger(const SoundBehavior &soundBehavior, const SoundShape *soundShape) :
		SoundTrigger(soundBehavior),
		soundShape(soundShape),
		isPlaying(false)
	{

	}

	ShapeTrigger::~ShapeTrigger()
	{
		delete soundShape;
	}

	SoundTrigger::TriggerType ShapeTrigger::getTriggerType() const
	{
		return SoundTrigger::SHAPE_TRIGGER;
	}

	SoundTrigger::TriggerResultValue ShapeTrigger::evaluateTrigger(const Point3<float> &listenerPosition)
	{
		if(!isPlaying && soundShape->pointInsidePlayShape(listenerPosition))
		{
			isPlaying = true;
			return getPlayTriggerValue();
		}else if(isPlaying && !soundShape->pointInsideStopShape(listenerPosition))
		{
			isPlaying = false;
			return getStopTriggerValue();
		}

		return SoundTrigger::NO_TRIGGER;
	}

	const SoundShape *ShapeTrigger::getSoundShape() const
	{
		return soundShape;
	}

	SoundTrigger::TriggerResultValue ShapeTrigger::getPlayTriggerValue()
	{
		if(getSoundBehavior().getPlayBehavior() == SoundBehavior::PLAY_LOOP)
		{
			return SoundTrigger::PLAY_LOOP;
		}

		return SoundTrigger::PLAY;
	}

	SoundTrigger::TriggerResultValue ShapeTrigger::getStopTriggerValue()
	{
		if(getSoundBehavior().getStopBehavior() == SoundBehavior::SMOOTH_STOP)
		{
			return SoundTrigger::SMOOTH_STOP;
		}

		return SoundTrigger::STOP;
	}

}
