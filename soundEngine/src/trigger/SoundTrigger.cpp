#include "trigger/SoundTrigger.h"

namespace urchin
{

	SoundTrigger::SoundTrigger(const SoundBehavior &soundBehavior) :
		soundBehavior(soundBehavior)
	{

	}

	SoundTrigger::~SoundTrigger()
	{

	}

	const SoundBehavior &SoundTrigger::getSoundBehavior() const
	{
		return soundBehavior;
	}

}
