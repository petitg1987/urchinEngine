#include "FrustumShadowData.h"

namespace urchin
{

	FrustumShadowData::FrustumShadowData(unsigned int frustumSplitIndex) :
			frustumSplitIndex(frustumSplitIndex),
			shadowCasterReceiverBoxUpdated(true),
			modelsMoved(true)
	{

	}

	FrustumShadowData::~FrustumShadowData()
	{

	}

	void FrustumShadowData::updateShadowCasterReceiverBox(const AABBox<float> &shadowCasterReceiverBox)
	{
		if(areIdenticalAABBox(shadowCasterReceiverBox, this->shadowCasterReceiverBox))
		{
			this->shadowCasterReceiverBoxUpdated = false;
		}else
		{
			this->shadowCasterReceiverBox = shadowCasterReceiverBox;
			this->lightProjectionMatrix = shadowCasterReceiverBox.toProjectionMatrix();

			this->shadowCasterReceiverBoxUpdated = true;
		}
	}

	const AABBox<float> &FrustumShadowData::getShadowCasterReceiverBox() const
	{
		return shadowCasterReceiverBox;
	}

	const Matrix4<float> &FrustumShadowData::getLightProjectionMatrix() const
	{
		return lightProjectionMatrix;
	}

	/**
	 * @return True when shadow caster and receiver box has been updated since previous frame
	 */
	bool FrustumShadowData::isShadowCasterReceiverBoxUpdated() const
	{
		return shadowCasterReceiverBoxUpdated;
	}

	/**
	 * @models Models visible from light in frustum split
	 */
	void FrustumShadowData::updateModels(const std::set<Model *> &models)
	{
		modelsMoved = false;

		for(std::set<Model *>::const_iterator it = models.begin(); it!=models.end(); ++it)
		{
			Model *model = *it;
			if(model->isMovingInOctree() || model->isAnimate())
			{
				modelsMoved = true;
			}
		}

		if(!modelsMoved && models!=this->models)
		{ //models have moved outside the frustum split OR new model move inside the frustum split
			modelsMoved = true;
		}

		this->models = models;
	}

	/**
	 * @return Models visible from light in frustum split
	 */
	const std::set<Model *> &FrustumShadowData::getModels() const
	{
		return models;
	}

	bool FrustumShadowData::isModelsMoved() const
	{
		return modelsMoved;
	}

	/**
	 * Return true when it's necessary to update the shadow map
	 */
	bool FrustumShadowData::needShadowMapUpdate() const
	{
		return modelsMoved || shadowCasterReceiverBoxUpdated;
	}

	bool FrustumShadowData::areIdenticalAABBox(const AABBox<float> &shadowCasterReceiverBox1, const AABBox<float> &shadowCasterReceiverBox2) const
	{
		constexpr float EPSILON = 0.0001f * 0.0001f;

		return shadowCasterReceiverBox1.getMin().squareDistance(shadowCasterReceiverBox2.getMin())<EPSILON
				&& shadowCasterReceiverBox1.getMax().squareDistance(shadowCasterReceiverBox2.getMax())<EPSILON;
	}

}
