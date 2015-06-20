#include <memory>
#include <stdexcept>
#include "UrchinCommon.h"

#include "LightScopeDisplayer.h"

namespace urchin
{

	LightScopeDisplayer::LightScopeDisplayer(SceneManager *sceneManager) :
		sceneManager(sceneManager)
	{

	}

	LightScopeDisplayer::~LightScopeDisplayer()
	{
		cleanCurrentDisplay();
	}

	void LightScopeDisplayer::displayLightScopeFor(const SceneLight *sceneLight)
	{
		cleanCurrentDisplay();

		if(sceneLight!=nullptr)
		{
			const Light *light = sceneLight->getLight();

			if(const OmnidirectionalLight *omnidirectionLight = dynamic_cast<const OmnidirectionalLight *>(light))
			{
				SphereModel *geometryModel = new SphereModel(omnidirectionLight->getSphereScope(), 25);
				lightScopeModels.push_back(geometryModel);
			}

			for(unsigned int i=0; i<lightScopeModels.size(); ++i)
			{
				lightScopeModels[i]->setColor(1.0, 0.0, 0.0);
				sceneManager->get3dRenderer()->addGeometry(lightScopeModels[i]);
			}
		}
	}

	void LightScopeDisplayer::cleanCurrentDisplay()
	{
		for(unsigned int i=0; i<lightScopeModels.size(); ++i)
		{
			sceneManager->get3dRenderer()->removeGeometry(lightScopeModels[i]);
			delete lightScopeModels[i];
		}

		lightScopeModels.clear();
	}
}
