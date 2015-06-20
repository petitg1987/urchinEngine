#include "sound/spatial/PointSound.h"

namespace urchin
{

	PointSound::PointSound(const std::string &filename, const Point3<float> &position) :
		Sound(filename),
		position(position),
		inaudibleDistance(ConfigService::instance()->getFloatValue("sound.defaultValue.inaudibleDistance"))
	{
		initializeSource();
	}

	/**
	 * @param inaudibleDistance Define distance at which the sound become inaudible
	 */
	PointSound::PointSound(const std::string &filename, const Point3<float> &position, float inaudibleDistance) :
		Sound(filename),
		position(position),
		inaudibleDistance(inaudibleDistance)
	{
		initializeSource();
	}

	PointSound::~PointSound()
	{

	}

	void PointSound::initializeSource()
	{
		alSourcei(getSourceId(), AL_SOURCE_RELATIVE, false);
		alSource3f(getSourceId(), AL_POSITION, position.X, position.Y, position.Z);

		alSourcef(getSourceId(), AL_MAX_DISTANCE, inaudibleDistance);
		alSourcef(getSourceId(), AL_REFERENCE_DISTANCE, 0.0);
		alSourcef(getSourceId(), AL_ROLLOFF_FACTOR, 1.0);
	}

	Sound::SoundType PointSound::getSoundType() const
	{
		return Sound::POINT;
	}

	void PointSound::setPosition(const Point3<float> &position)
	{
		this->position = position;
		alSource3f(getSourceId(), AL_POSITION, position.X, position.Y, position.Z);
	}

	Point3<float> PointSound::getPosition() const
	{
		return position;
	}

	/**
	 * @param inaudibleDistance Inaudible distance of the sound
	 */
	void PointSound::setInaudibleDistance(float inaudibleDistance)
	{
		this->inaudibleDistance = inaudibleDistance;
		alSourcef(getSourceId(), AL_MAX_DISTANCE, inaudibleDistance);
	}

	/**
	 * @return Distance at which the sound become inaudible
	 */
	float PointSound::getInaudibleDistance() const
	{
		return inaudibleDistance;
	}

}
