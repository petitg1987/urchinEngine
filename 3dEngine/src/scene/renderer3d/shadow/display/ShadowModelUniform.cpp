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
		unsigned int layersToUpdate = 0;

		for(unsigned int i=0; i<shadowData->getNbFrustumShadowData(); ++i)
		{
			const FrustumShadowData *frustumShadowData = shadowData->getFrustumShadowData(i);
			if(frustumShadowData->needShadowMapUpdate() && frustumShadowData->getModels().lower_bound((Model *)model)!=frustumShadowData->getModels().end())
			{
                layersToUpdate = layersToUpdate | MathAlgorithm::powerOfTwo(i);
			}
		}

		glUniform1ui(layersToUpdateLoc, layersToUpdate);
	}

}
