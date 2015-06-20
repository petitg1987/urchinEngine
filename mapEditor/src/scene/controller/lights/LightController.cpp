#include <stdexcept>

#include "LightController.h"

namespace urchin
{

	LightController::LightController(MapHandler *mapHandler) :
			bIsModified(false),
			mapHandler(mapHandler)
	{

	}

	LightController::~LightController()
	{

	}

	bool LightController::isModified() const
	{
		return bIsModified;
	}

	void LightController::markModified()
	{
		bIsModified = true;
	}

	void LightController::resetModified()
	{
		bIsModified = false;
	}

	std::list<const SceneLight *> LightController::getSceneLights() const
	{
		std::list<SceneLight *> sceneLights = mapHandler->getMap()->getSceneLights();
		std::list<const SceneLight *> constSceneLights;
		constSceneLights.insert(constSceneLights.begin(), sceneLights.begin(), sceneLights.end());

		return constSceneLights;
	}

	void LightController::addSceneLight(SceneLight *sceneLight)
	{
		mapHandler->getMap()->addSceneLight(sceneLight);

		markModified();
	}

	void LightController::removeSceneLight(const SceneLight *constSceneLight)
	{
		SceneLight *sceneLight = findSceneLight(constSceneLight);
		mapHandler->getMap()->removeSceneLight(sceneLight);

		markModified();
	}

	const SceneLight *LightController::updateSceneLightGeneralProperties(const SceneLight *constSceneLight,
			const Point3<float> &ambientColor, bool isProduceShadow)
	{
		SceneLight *sceneLight = findSceneLight(constSceneLight);
		Light *light = sceneLight->getLight();

		light->setAmbientColor(ambientColor);
		if(light->isProduceShadow()!=isProduceShadow)
		{
			light->setProduceShadow(isProduceShadow);
		}

		markModified();
		return sceneLight;
	}

	const SceneLight *LightController::updateSceneOmnidirectionalLightProperties(const SceneLight *constSceneLight,
			float attenuation, const Point3<float> &position)
	{
		SceneLight *sceneLight = findSceneLight(constSceneLight);
		OmnidirectionalLight *light = static_cast<OmnidirectionalLight *>(sceneLight->getLight());

		light->setAttenuation(attenuation);
		light->setPosition(position);

		markModified();
		return sceneLight;
	}

	const SceneLight *LightController::updateSceneSunLightProperties(const SceneLight *constSceneLight, const Vector3<float> &direction)
	{
		SceneLight *sceneLight = findSceneLight(constSceneLight);
		SunLight *light = static_cast<SunLight *>(sceneLight->getLight());

		light->setDirection(direction);

		markModified();
		return sceneLight;
	}

	SceneLight *LightController::findSceneLight(const SceneLight *constSceneLight)
	{
		std::list<SceneLight *> sceneLights = mapHandler->getMap()->getSceneLights();
		std::list<SceneLight *>::iterator it = std::find(sceneLights.begin(), sceneLights.end(), constSceneLight);

		if(it!=sceneLights.end())
		{
			return *it;
		}

		throw std::invalid_argument("Impossible to find scene light: " + constSceneLight->getName());
	}

}
