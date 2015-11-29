#include <GL/gl.h>

#include "ShadowModelUniform.h"

namespace urchin
{

	ShadowModelUniform::ShadowModelUniform() :
			CustomModelUniform(),
			splitsToUpdateLoc(0),
			shadowData(nullptr)
	{

	}

	ShadowModelUniform::~ShadowModelUniform()
	{

	}

	void ShadowModelUniform::setSplitsToUpdateLocation(int splitsToUpdateLoc)
	{
		this->splitsToUpdateLoc = splitsToUpdateLoc;
	}

	void ShadowModelUniform::setModelUniformData(const ShadowData *shadowData)
	{
		this->shadowData = shadowData;
	}

	void ShadowModelUniform::loadCustomUniforms(const Model *model)
	{
		constexpr unsigned int POWER_TWO_TAB[13] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
		unsigned int nbSplits = shadowData->getNbFrustumShadowData();
		unsigned int splitsToUpdate = 0;

		for(unsigned int i=0; i<nbSplits; ++i)
		{
			const FrustumShadowData *frustumShadowData = shadowData->getFrustumShadowData(i);
			if(frustumShadowData->isShadowCasterReceiverBoxUpdated() || frustumShadowData->isModelsMoved())
			{
				const std::set<Model *> &frustumModels = frustumShadowData->getModels();
				std::set<Model *>::const_iterator itModel = std::find(frustumModels.begin(), frustumModels.end(), model);

				if(itModel!=frustumModels.end())
				{
					splitsToUpdate = splitsToUpdate | POWER_TWO_TAB[i];
				}
			}
		}

		glUniform1ui(splitsToUpdateLoc, splitsToUpdate);
	}

}
