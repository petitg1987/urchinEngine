#include <GL/glew.h>
#include <GL/gl.h>

#include "ShadowModelUniform.h"

namespace urchin
{

	ShadowModelUniform::ShadowModelUniform() :
			CustomModelUniform(),
			layersToUpdateLoc(0),
			shadowData(nullptr)
	{

	}

	ShadowModelUniform::~ShadowModelUniform()
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
				std::set<Model *>::const_iterator itModel = std::find(frustumModels.begin(), frustumModels.end(), model);

				if(itModel!=frustumModels.end())
				{
					layersToUpdate = layersToUpdate | MathAlgorithm::powerOfTwo(i);
				}
			}
		}

		glUniform1ui(layersToUpdateLoc, layersToUpdate);
	}

}
