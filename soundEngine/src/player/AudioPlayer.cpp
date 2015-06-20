#include "player/AudioPlayer.h"

namespace urchin
{

	AudioPlayer::AudioPlayer(const Sound *sound) :
		sound(sound)
	{

	}

	AudioPlayer::~AudioPlayer()
	{

	}

	const Sound *AudioPlayer::getSound() const
	{
		return sound;
	}

}
