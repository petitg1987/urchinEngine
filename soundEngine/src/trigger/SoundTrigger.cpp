#include "trigger/SoundTrigger.h"

namespace urchin
{

	SoundTrigger::SoundTrigger(const SoundBehavior &soundBehavior) :
		soundBehavior(soundBehavior)
	{

	}

	const SoundBehavior &SoundTrigger::getSoundBehavior() const
	{
		return soundBehavior;
	}

}
