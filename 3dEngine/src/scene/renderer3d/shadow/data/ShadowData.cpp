#include <cmath>
#include <stdexcept>

#include "ShadowData.h"
#include "scene/renderer3d/light/Light.h"

namespace urchin
{

	ShadowData::ShadowData(const Light *const light, unsigned int nbFrustumsplit) :
			light(light),
			fboID(0),
			depthTextureID(0),
			shadowMapTextureID(0),
			downSampleFilter(nullptr),
			blurFilter(nullptr)
	{
		for(unsigned int frustumSplitIndex =0; frustumSplitIndex<nbFrustumsplit; ++frustumSplitIndex)
		{
			frustumShadowData.push_back(new FrustumShadowData(frustumSplitIndex));
		}
	}

	ShadowData::~ShadowData()
	{
		for(unsigned int i=0; i<frustumShadowData.size(); ++i)
		{
			delete frustumShadowData[i];
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

	void ShadowData::setDownSampleFilter(std::shared_ptr<const TextureFilter> downSampleFilter)
	{
		this->downSampleFilter = downSampleFilter;
	}

	std::shared_ptr<const TextureFilter> ShadowData::getDownSampleFilter() const
	{
		return downSampleFilter;
	}

	void ShadowData::setBlurFilter(std::shared_ptr<const TextureFilter> blurFilter)
	{
		this->blurFilter = blurFilter;
	}

	std::shared_ptr<const TextureFilter> ShadowData::getBlurFilter() const
	{
		return blurFilter;
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

	std::set<Model *> ShadowData::retrieveModels() const
	{
		std::set<Model *> models;
		for(unsigned int i=0; i<getNbFrustumShadowData(); ++i)
		{
			const std::set<Model *> &frustumsplitModels = getFrustumShadowData(i)->getModels();
			models.insert(frustumsplitModels.begin(), frustumsplitModels.end());
		}
		return models;
	}
}
