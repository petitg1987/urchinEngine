#include <GL/glew.h>
#include <algorithm>

#include "ShadowModelUniform.h"

namespace urchin
{

	ShadowModelUniform::ShadowModelUniform() :
			CustomModelUniform(),
			layersToUpdateLoc(0),
			shadowData(nullptr)
	{

	}

	void ShadowModelUniform::setLayersToUpdateLocation(int layersToUpdateLoc)
	{
		this->layersToUpdateLoc = layersToUpdateLoc;
	}

	void ShadowModelUniform::setModelUniformData(const ShadowData *shadowData)
	{
		this->shadowData = shadowData;
	}

	void ShadowModelUniform::loadCustomUniforms(const Model *model)
	{
		unsigned int nbLayers = shadowData->getNbFrustumShadowData();
		unsigned int layersToUpdate = 0;

		for(unsigned int i=0; i<nbLayers; ++i)
		{
			const FrustumShadowData *frustumShadowData = shadowData->getFrustumShadowData(i);
			if(frustumShadowData->needShadowMapUpdate())
			{
				const std::set<Model *> &frustumModels = frustumShadowData->getModels();
				auto itModel = std::find(frustumModels.begin(), frustumModels.end(), model);

				if(itModel!=frustumModels.end())
				{
					layersToUpdate = layersToUpdate | MathAlgorithm::powerOfTwo(i);
				}
			}
		}

		glUniform1ui(layersToUpdateLoc, layersToUpdate);
	}

}
