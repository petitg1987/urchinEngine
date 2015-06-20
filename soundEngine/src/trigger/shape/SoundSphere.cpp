#include "trigger/shape/SoundSphere.h"

namespace urchin
{

	/**
	 * @param margin Margin to sphere used to determine stop sound trigger
	 */
	SoundSphere::SoundSphere(float radius, const Point3<float> &position, float margin) :
		margin(margin),
		playTriggerSphere(Sphere<float>(radius, position)),
		stopTriggerSphere(Sphere<float>(radius + margin, position))
	{

	}

	SoundSphere::~SoundSphere()
	{

	}

	SoundShape::ShapeType SoundSphere::getShapeType() const
	{
		return SoundShape::SPHERE_SHAPE;
	}

	const Sphere<float> &SoundSphere::getPlayTriggerSphere() const
	{
		return playTriggerSphere;
	}

	const Sphere<float> &SoundSphere::getStopTriggerSphere() const
	{
		return stopTriggerSphere;
	}

	/**
	 * @return Margin between shape trigger play event and trigger stop event
	 */
	float SoundSphere::getMargin() const
	{
		return margin;
	}

	float SoundSphere::getRadius() const
	{
		return playTriggerSphere.getRadius();
	}

	const Point3<float> SoundSphere::getPosition() const
	{
		return playTriggerSphere.getPosition();
	}

	bool SoundSphere::pointInsidePlayShape(const Point3<float> &point) const
	{
		return playTriggerSphere.collideWithPoint(point);
	}

	bool SoundSphere::pointInsideStopShape(const Point3<float> &point) const
	{
		return stopTriggerSphere.collideWithPoint(point);
	}

	SoundShape *SoundSphere::clone() const
	{
		return new SoundSphere(getRadius(), getPosition(), getMargin());
	}

}
