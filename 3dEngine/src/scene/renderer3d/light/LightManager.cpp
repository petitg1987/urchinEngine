#include <GL/glew.h>
#include <stdexcept>

#include "LightManager.h"
#include "scene/renderer3d/light/sun/SunLight.h"
#include "scene/renderer3d/light/omnidirectional/OmnidirectionalLight.h"

#define DEFAULT_OCTREE_DEPTH 2

namespace urchin
{

	LightManager::LightManager() :
			lastUpdatedLight(nullptr),
			maxLights(ConfigService::instance()->getUnsignedIntValue("light.maxLights")),
			globalAmbientColorLoc(0),
			globalAmbientColor(Point4<float>(0.0, 0.0, 0.0, 0.0))
	{
		lightsInfo = new LightInfo[maxLights];
		lightOctreeManager = new OctreeManager<Light>(DEFAULT_OCTREE_DEPTH);
	}

	LightManager::~LightManager()
	{
		std::set<Light *> allOctreeableLights = lightOctreeManager->getOctreeables();
		for (auto allOctreeableLight : allOctreeableLights)
        {
			delete allOctreeableLight;
		}
		for(std::vector<Light *>::const_iterator it=parallelBeamsLights.begin(); it!=parallelBeamsLights.end(); ++it)
		{
			delete (*it);
		}


		delete lightOctreeManager;
		delete [] lightsInfo;
	}

	void LightManager::loadUniformLocationFor(unsigned int deferredShaderID)
	{
		std::ostringstream isExistLocName, produceShadowLocName, hasParallelBeamsName, positionOrDirectionLocName;
		std::ostringstream exponentialAttName, lightAmbientName;
		for(unsigned int i=0;i<maxLights;++i)
		{
			isExistLocName.str("");
			isExistLocName << "lightsInfo[" << i << "].isExist";

			produceShadowLocName.str("");
			produceShadowLocName << "lightsInfo[" << i << "].produceShadow";

			hasParallelBeamsName.str("");
			hasParallelBeamsName << "lightsInfo[" << i << "].hasParallelBeams";

			positionOrDirectionLocName.str("");
			positionOrDirectionLocName << "lightsInfo[" << i << "].positionOrDirection";

			exponentialAttName.str("");
			exponentialAttName << "lightsInfo[" << i << "].exponentialAttenuation";

			lightAmbientName.str("");
			lightAmbientName << "lightsInfo[" << i << "].lightAmbient";

			lightsInfo[i].isExistLoc = glGetUniformLocation(deferredShaderID, isExistLocName.str().c_str());
			lightsInfo[i].produceShadowLoc = glGetUniformLocation(deferredShaderID, produceShadowLocName.str().c_str());
			lightsInfo[i].hasParallelBeamsLoc = glGetUniformLocation(deferredShaderID, hasParallelBeamsName.str().c_str());
			lightsInfo[i].positionOrDirectionLoc = glGetUniformLocation(deferredShaderID, positionOrDirectionLocName.str().c_str());

			lightsInfo[i].exponentialAttLoc = glGetUniformLocation(deferredShaderID, exponentialAttName.str().c_str());
			lightsInfo[i].lightAmbientLoc = glGetUniformLocation(deferredShaderID, lightAmbientName.str().c_str());
		}

		globalAmbientColorLoc = glGetUniformLocation(deferredShaderID, "globalAmbient");
	}

	OctreeManager<Light> *LightManager::getLightOctreeManager() const
	{
		return lightOctreeManager;
	}

	void LightManager::onLightEvent(Light *light, NotificationType notificationType)
	{
		lastUpdatedLight = light;
		notifyObservers(this, notificationType);
	}

	Light *LightManager::getLastUpdatedLight()
	{
		return lastUpdatedLight;
	}

	/**
	 * @return Maximum of lights authorized to affect the scene in the same time
	 */
	unsigned int LightManager::getMaxLights() const
	{
		return maxLights;
	}

	const std::vector<Light *> &LightManager::getVisibleLights() const
	{
		return visibleLights;
	}

	void LightManager::addLight(Light *light)
	{
		if(light!=nullptr)
		{
			if(light->hasParallelBeams())
			{
				parallelBeamsLights.push_back(light);
			}else
			{
				lightOctreeManager->addOctreeable(light);
			}

			onLightEvent(light, LightManager::ADD_LIGHT);
		}
	}

	void LightManager::removeLight(Light *light)
	{
		if(light!=nullptr)
		{
			if(light->hasParallelBeams())
			{
				auto it = std::find(parallelBeamsLights.begin(), parallelBeamsLights.end(), light);
				parallelBeamsLights.erase(it);
			}else
			{
				lightOctreeManager->removeOctreeable(light);
			}

			onLightEvent(light, LightManager::REMOVE_LIGHT);

			delete light;
		}
	}

	void LightManager::setGlobalAmbientColor(const Point4<float> &globalAmbientColor)
	{
		this->globalAmbientColor = globalAmbientColor;
	}

	const Point4<float> &LightManager::getGlobalAmbientColor() const
	{
		return globalAmbientColor;
	}

	void LightManager::updateLights(const Frustum<float> &frustum)
	{
		ScopeProfiler profiler("3d", "updateLights");

		lightOctreeManager->refreshOctreeables();
		std::set<Light *> lightInFrustum = lightOctreeManager->getOctreeablesIn(frustum);

		visibleLights.clear();
		visibleLights.assign(lightInFrustum.begin(), lightInFrustum.end());
		visibleLights.insert(visibleLights.end(), parallelBeamsLights.begin(), parallelBeamsLights.end());
	}

	void LightManager::loadLights()
	{
		const std::vector<Light *> &lights = getVisibleLights();
		for(unsigned int i=0;i<maxLights; ++i)
		{
			if(lights.size()>i)
			{
				const Light *light = lights[i];

				glUniform1i(lightsInfo[i].isExistLoc, true);
				glUniform1i(lightsInfo[i].produceShadowLoc, light->isProduceShadow());
				glUniform1i(lightsInfo[i].hasParallelBeamsLoc, light->hasParallelBeams());
				if(lights[i]->getLightType()==Light::SUN)
				{
					const auto *sunLight = dynamic_cast<const SunLight *>(light);

					glUniform3fv(lightsInfo[i].positionOrDirectionLoc, 1, (const float *)sunLight->getDirections()[0]);
				}else if(lights[i]->getLightType()==Light::OMNIDIRECTIONAL)
				{
					const auto *omnidirectionalLight = dynamic_cast<const OmnidirectionalLight *>(light);

					glUniform3fv(lightsInfo[i].positionOrDirectionLoc, 1, (const float *)omnidirectionalLight->getPosition());
					glUniform1f(lightsInfo[i].exponentialAttLoc, omnidirectionalLight->getExponentialAttenuation());
				}else
				{
					throw std::invalid_argument("Unknown light type to load uniform: " + std::to_string(light->getLightType()));
				}

				glUniform3fv(lightsInfo[i].lightAmbientLoc, 1, (const float *)light->getAmbientColor());
			}else
			{
				glUniform1i(lightsInfo[i].isExistLoc, false);
				break;
			}
		}

		glUniform4fv(globalAmbientColorLoc, 1, (const float *)getGlobalAmbientColor());
	}

	void LightManager::postUpdateLights()
	{
		lightOctreeManager->postRefreshOctreeables();
	}

#ifdef _DEBUG
	void LightManager::drawLightOctree(const Matrix4<float> &projectionMatrix, const Matrix4<float> &viewMatrix) const
	{
		lightOctreeManager->drawOctree(projectionMatrix, viewMatrix);
	}
#endif

}
