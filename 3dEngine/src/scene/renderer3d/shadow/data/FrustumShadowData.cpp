#include "FrustumShadowData.h"

namespace urchin
{

	FrustumShadowData::FrustumShadowData(unsigned int frustumSplitIndex) :
			frustumSplitIndex(frustumSplitIndex)
	{

	}

	FrustumShadowData::~FrustumShadowData()
	{

	}

	void FrustumShadowData::setShadowCasterReceiverBox(const AABBox<float> &shadowCasterReceiverBox)
	{
		this->shadowCasterReceiverBox = shadowCasterReceiverBox;
	}

	const AABBox<float> &FrustumShadowData::getShadowCasterReceiverBox() const
	{
		return shadowCasterReceiverBox;
	}

	void FrustumShadowData::setLightProjectionMatrix(const Matrix4<float> &lightProjectionMatrix)
	{
		this->lightProjectionMatrix = lightProjectionMatrix;
	}

	const Matrix4<float> &FrustumShadowData::getLightProjectionMatrix() const
	{
		return lightProjectionMatrix;
	}

	/**
	 * @models Models visible from light in frustum split
	 */
	void FrustumShadowData::setModels(const std::set<Model *> &models)
	{
		this->models = models;
	}

	/**
	 * @return Models visible from light in frustum split
	 */
	const std::set<Model *> &FrustumShadowData::getModels() const
	{
		return models;
	}

}
