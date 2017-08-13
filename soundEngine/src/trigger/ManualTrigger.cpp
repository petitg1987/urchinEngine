#include <stdexcept>

#include "trigger/ManualTrigger.h"

namespace urchin
{

	ManualTrigger::ManualTrigger(const SoundBehavior &soundBehavior) :
		SoundTrigger(soundBehavior),
		manualTriggerValue(ManualTriggerValue::STOP)
	{

	}

	SoundTrigger::TriggerType ManualTrigger::getTriggerType() const
	{
		return SoundTrigger::MANUAL_TRIGGER;
	}

	void ManualTrigger::play()
	{
		if(getSoundBehavior().getPlayBehavior() == SoundBehavior::PLAY_LOOP)
		{
			manualTriggerValue = ManualTriggerValue::PLAY_LOOP;
		}else
		{
			manualTriggerValue = ManualTriggerValue::PLAY;
		}
	}

	void ManualTrigger::stop()
	{
		if(getSoundBehavior().getStopBehavior() == SoundBehavior::SMOOTH_STOP)
		{
			manualTriggerValue = ManualTriggerValue::SMOOTH_STOP;
		}else
		{
			manualTriggerValue = ManualTriggerValue::STOP;
		}
	}

	void ManualTrigger::pause()
	{
		manualTriggerValue = ManualTriggerValue::PAUSE;
	}

	SoundTrigger::TriggerResultValue ManualTrigger::evaluateTrigger(const Point3<float> &listenerPosition)
	{
		SoundTrigger::TriggerResultValue result;

		if(manualTriggerValue == ManualTriggerValue::PLAY)
		{
			result = SoundTrigger::PLAY;
		}else if(manualTriggerValue == ManualTriggerValue::PLAY_LOOP)
		{
			result = SoundTrigger::PLAY_LOOP;
		}else if(manualTriggerValue == ManualTriggerValue::STOP)
		{
			result = SoundTrigger::STOP;
		}else if(manualTriggerValue == ManualTriggerValue::SMOOTH_STOP)
		{
			result = SoundTrigger::SMOOTH_STOP;
		}else if(manualTriggerValue == ManualTriggerValue::PAUSE)
		{
			result = SoundTrigger::PAUSE;
		}else if(manualTriggerValue == ManualTriggerValue::NO_TRIGGER)
		{
			result = SoundTrigger::NO_TRIGGER;
		}else
		{
			throw std::invalid_argument("Unknown manual trigger value: " + std::to_string(manualTriggerValue));
		}

		manualTriggerValue = NO_TRIGGER;
		return result;
	}

}
