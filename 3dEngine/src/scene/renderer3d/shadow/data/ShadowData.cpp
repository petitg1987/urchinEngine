#include <GL/glew.h>
#include <cmath>
#include <stdexcept>

#include "ShadowData.h"

namespace urchin
{

	ShadowData::ShadowData(const Light *light, unsigned int nbFrustumSplit) :
			light(light),
			fboID(0),
			depthTextureID(0),
			shadowMapTextureID(0)
	{
		for(unsigned int frustumSplitIndex =0; frustumSplitIndex<nbFrustumSplit; ++frustumSplitIndex)
		{
			frustumShadowData.push_back(new FrustumShadowData(frustumSplitIndex));
		}
	}

	ShadowData::~ShadowData()
	{
		for (auto &fsd : frustumShadowData)
		{
			delete fsd;
		}

		glDeleteFramebuffers(1, &fboID);
		glDeleteTextures(1, &depthTextureID);
		glDeleteTextures(1, &shadowMapTextureID);
	}

	void ShadowData::setFboID(unsigned int fboID)
	{
		this->fboID = fboID;
	}

	unsigned int ShadowData::getFboID() const
	{
		return fboID;
	}

	void ShadowData::setDepthTextureID(unsigned int depthTextureID)
	{
		this->depthTextureID = depthTextureID;
	}

	unsigned int ShadowData::getDepthTextureID() const
	{
		return depthTextureID;
	}

	/**
	 * @param shadowMapTextureID Shadow map texture ID (variance shadow map)
	 */
	void ShadowData::setShadowMapTextureID(unsigned int shadowMapTextureID)
	{
		this->shadowMapTextureID = shadowMapTextureID;
	}

	/**
	 * Returns shadow map texture ID (variance shadow map)
	 */
	unsigned int ShadowData::getShadowMapTextureID() const
	{
		return shadowMapTextureID;
	}

	void ShadowData::addTextureFilter(const std::shared_ptr<const TextureFilter> &textureFilter)
	{
		textureFilters.push_back(textureFilter);
	}

	void ShadowData::applyTextureFilters()
	{
		unsigned int layersToUpdate = 0;
		for(unsigned int i=0; i<getNbFrustumShadowData(); ++i)
		{
			if(getFrustumShadowData(i)->needShadowMapUpdate())
			{
				layersToUpdate = layersToUpdate | MathAlgorithm::powerOfTwo(i);
			}
		}

		unsigned int textureId = shadowMapTextureID;
		for(auto &textureFilter : textureFilters)
		{
			textureFilter->applyOn(textureId, layersToUpdate);
			textureId = textureFilter->getTextureID();
		}
	}

	unsigned int ShadowData::getFilteredShadowMapTextureID() const
	{
		unsigned int textureFiltersSize = textureFilters.size();
		if(textureFiltersSize == 0)
		{
			return shadowMapTextureID;
		}

		return textureFilters[textureFiltersSize-1]->getTextureID();
	}

	void ShadowData::setLightViewMatrix(const Matrix4<float> &lightViewMatrix)
	{
		this->lightViewMatrix = lightViewMatrix;
	}

	const Matrix4<float> &ShadowData::getLightViewMatrix() const
	{
		return lightViewMatrix;
	}

	unsigned int ShadowData::getNbFrustumShadowData() const
	{
		return frustumShadowData.size();
	}

	/**
	 * @param index Index of frustum split (0: frustum split nearest to eye)
	 */
	FrustumShadowData *ShadowData::getFrustumShadowData(unsigned int index)
	{
		return frustumShadowData[index];
	}

	/**
	 * @param index Index of frustum split (0: frustum split nearest to eye)
	 */
	const FrustumShadowData *ShadowData::getFrustumShadowData(unsigned int index) const
	{
		return frustumShadowData[index];
	}

	std::unordered_set<Model *> ShadowData::retrieveModels() const
	{
		std::unordered_set<Model *> models;
		for(unsigned int i=0; i<getNbFrustumShadowData(); ++i)
		{
			if(getFrustumShadowData(i)->needShadowMapUpdate())
			{
				const std::unordered_set<Model *> &frustumSplitModels = getFrustumShadowData(i)->getModels();
				models.insert(frustumSplitModels.begin(), frustumSplitModels.end());
			}
		}
		return models;
	}
}
