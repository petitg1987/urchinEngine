#include <GL/gl.h>

#include "ShadowModelUniform.h"

namespace urchin
{

	ShadowModelUniform::ShadowModelUniform() :
			CustomModelUniform(),
			firstSplitLoc(0),
			lastSplitLoc(0),
			shadowData(nullptr)
	{

	}

	ShadowModelUniform::~ShadowModelUniform()
	{

	}

	void ShadowModelUniform::setFirstSplitLocation(int firstSplitLoc)
	{
		this->firstSplitLoc = firstSplitLoc;
	}

	void ShadowModelUniform::setLastSplitLocation(int lastSplitLoc)
	{
		this->lastSplitLoc = lastSplitLoc;
	}

	void ShadowModelUniform::setModelUniformData(const ShadowData *shadowData)
	{
		this->shadowData = shadowData;
	}

	void ShadowModelUniform::loadCustomUniforms(const Model *model)
	{
		//We compute a [firstSplit, lastSplit] representing the first and last shadow map where the model need to be renderer.
		//Note: splits are not necessarily consecutive in some rare cases: split X can contain model because it's a shadow caster and split X+2
		//can contain model because it's a shadow receiver.

		int firstSplit = 0;
		int lastSplit = -1;
		bool firstSplitFound = false;

		for(unsigned int i=0; i<shadowData->getNbFrustumShadowData(); ++i)
		{
			const std::set<Model *> &frustumModels = shadowData->getFrustumShadowData(i)->getModels();
			std::set<Model *>::const_iterator itModel = std::find(frustumModels.begin(), frustumModels.end(), model);

			if(itModel!=frustumModels.end())
			{
				if(!firstSplitFound)
				{
					firstSplit = i;
					firstSplitFound = true;
				}
				lastSplit = i;
			}
		}

		glUniform1i(firstSplitLoc, firstSplit);
		glUniform1i(lastSplitLoc, lastSplit);
	}

}
