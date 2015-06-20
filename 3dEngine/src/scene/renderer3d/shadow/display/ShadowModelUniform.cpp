#include <GL/glew.h>
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
		//Determinate when models should appears on the splitted shadow maps.
		//We compute a [firstSplit, lastSplit] representing the first and last shadow map where the model need to be renderer.
		//For performance purpose, we start by last split. Last bounding box is biggest and so it's increase the chance the have the model on it.

		int firstSplit = 0;
		int lastSplit = -1;
		bool lastSplitFound = false;

		for(int i=shadowData->getNbFrustumShadowData()-1; i>=0; --i)
		{
			const std::set<Model *> &frustumModels = shadowData->getFrustumShadowData(i)->getModels();
			std::set<Model *>::const_iterator itModel = std::find(frustumModels.begin(), frustumModels.end(), model);

			if(itModel!=frustumModels.end())
			{
				if(!lastSplitFound)
				{
					lastSplit = i;
					lastSplitFound = true;
				}
			}else if(lastSplitFound)
			{
				firstSplit = i + 1;
				break;
			}
		}

		glUniform1i(firstSplitLoc, firstSplit);
		glUniform1i(lastSplitLoc, lastSplit);
	}

}
