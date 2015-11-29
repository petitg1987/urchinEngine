#include <stdexcept>

#include "SunLight.h"

namespace urchin
{

	SunLight::SunLight(const Vector3<float> &direction) :
		Light(),
		direction(direction)
	{

	}

	SunLight::~SunLight()
	{

	}

	void SunLight::setDirection(const Vector3<float> &direction)
	{
		this->direction = direction;

		notifyObservers(this, Light::LIGHT_MOVE);
	}

	std::vector<Vector3<float>> SunLight::getDirections() const
	{
		std::vector<Vector3<float>> directions;
		directions.push_back(Vector3<float>(direction.X, direction.Y, direction.Z));

		return directions;
	}

	const Point3<float> &SunLight::getPosition() const
	{
		throw std::runtime_error("No position for sun light.");
	}

	Light::LightType SunLight::getLightType() const
	{
		return SUN;
	}

	bool SunLight::hasParallelBeams() const
	{
		return true;
	}

	const AABBox<float> &SunLight::getAABBox() const
	{
		throw std::runtime_error("No bounding box for sun light.");
	}

}
