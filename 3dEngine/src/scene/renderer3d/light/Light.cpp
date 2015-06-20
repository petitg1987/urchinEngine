#include <stdexcept>

#include "Light.h"

namespace urchin
{

	Light::Light() :
			ambientColor(Point3<float>(0.0, 0.0, 0.0)),
			produceShadow(false)
	{

	}

	Light::~Light()
	{

	}

	const Transform<float> &Light::getTransform() const
	{
		//Null transformation because light position is specified in global space
		static Transform<float> transform;
		return transform;
	}

	void Light::setProduceShadow(bool produceShadow)
	{
		this->produceShadow = produceShadow;

		notifyObservers(this, Light::PRODUCE_SHADOW);
	}

	bool Light::isProduceShadow() const
	{
		return produceShadow;
	}

	void Light::setAmbientColor(const Point3<float> &ambientColor)
	{
		this->ambientColor = ambientColor;
	}

	const Point3<float> &Light::getAmbientColor() const
	{
		return ambientColor;
	}

}
