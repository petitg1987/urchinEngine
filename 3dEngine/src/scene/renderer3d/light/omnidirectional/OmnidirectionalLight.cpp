#include <stdexcept>
#include <cmath>
#include <memory>

#include "OmnidirectionalLight.h"
#include "utils/display/geometry/sphere/SphereModel.h"

namespace urchin
{

	OmnidirectionalLight::OmnidirectionalLight(const Point3<float> &position) :
			Light(),
			position(position),
			attenuationNoEffect(ConfigService::instance()->getFloatValue("light.attenuationNoEffect")),
			exponentialAttenuation(0.01),
			sphereScope(nullptr),
			bboxScope(nullptr)
	{
		computeScope();
	}

	OmnidirectionalLight::~OmnidirectionalLight()
	{
		delete sphereScope;
		delete bboxScope;
	}

	void OmnidirectionalLight::setPosition(const Point3<float> &position)
	{
		this->position = position;

		computeScope();

		notifyObservers(this, Light::LIGHT_MOVE);
	}

	const Point3<float> &OmnidirectionalLight::getPosition() const
	{
		return position;
	}

	std::vector<Vector3<float>> OmnidirectionalLight::getDirections() const
	{
		std::vector<Vector3<float>> directions;

		directions.push_back(Vector3<float>(1.0, 0.0, 0.0)); //Left
		directions.push_back(Vector3<float>(-1.0, 0.0, 0.0)); //Right
		directions.push_back(Vector3<float>(0.0, 1.0, 0.0)); //Top
		directions.push_back(Vector3<float>(0.0, -1.0, 0.0)); //Bottom
		directions.push_back(Vector3<float>(0.0, 0.0, 1.0)); //Back
		directions.push_back(Vector3<float>(0.0, 0.0, -1.0)); //Front

		return directions;
	}

	Light::LightType OmnidirectionalLight::getLightType() const
	{
		return OMNIDIRECTIONAL;
	}

	bool OmnidirectionalLight::hasParallelBeams() const
	{
		return false;
	}

	const AABBox<float> &OmnidirectionalLight::getAABBox() const
	{
		return *bboxScope;
	}

	void OmnidirectionalLight::setAttenuation(float exponentialAttenuation)
	{
		if(exponentialAttenuation < 0.0f)
		{
			throw std::domain_error("Exponential attenuation must be greater or equals to zero.");
		}
		this->exponentialAttenuation = exponentialAttenuation;

		computeScope();
	}

	float OmnidirectionalLight::getExponentialAttenuation() const
	{
		return exponentialAttenuation;
	}

	const AABBox<float> &OmnidirectionalLight::getAABBoxScope() const
	{
		return *bboxScope;
	}

	const Sphere<float> &OmnidirectionalLight::getSphereScope() const
	{
		return *sphereScope;
	}

	/**
	 * Computes the sphere scope representing light affectation zone
	 */
	void OmnidirectionalLight::computeScope()
	{
		delete sphereScope;
		delete bboxScope;

		float radius = -std::log(attenuationNoEffect) / getExponentialAttenuation();
		sphereScope = new Sphere<float>(radius, getPosition());
		bboxScope = new AABBox<float>(getPosition()-radius, getPosition()+radius);

		notifyOctreeableMove();
	}

}
