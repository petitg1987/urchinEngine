#include "FrustumShadowData.h"

namespace urchin
{

	FrustumShadowData::FrustumShadowData(unsigned int frustumSplitIndex, float shadowMapFrequencyUpdate) :
			frustumSplitIndex(frustumSplitIndex),
			shadowCasterReceiverBoxUpdated(false),
			modelsMoved(false),
			modelsInverseFrequencyUpdate(round(1.0f/shadowMapFrequencyUpdate)),
			modelsMovedCount(0),
			modelsRequiredUpdate(false)
	{

	}

	FrustumShadowData::~FrustumShadowData()
	{

	}

	void FrustumShadowData::updateShadowCasterReceiverBox(const AABBox<float> &shadowCasterReceiverBox, bool forceUpdateAllShadowMap)
	{
		if(areIdenticalAABBox(shadowCasterReceiverBox, this->shadowCasterReceiverBox) && !forceUpdateAllShadowMap)
		{
			this->shadowCasterReceiverBoxUpdated = false;
		}else
		{
			this->shadowCasterReceiverBox = shadowCasterReceiverBox;
			this->lightProjectionMatrix = shadowCasterReceiverBox.toProjectionMatrix();

			this->shadowCasterReceiverBoxUpdated = true;
		}
	}

	bool FrustumShadowData::areIdenticalAABBox(const AABBox<float> &shadowCasterReceiverBox1, const AABBox<float> &shadowCasterReceiverBox2) const
	{
		constexpr float SQUARE_EPSILON = 0.0001f * 0.0001f;

		return shadowCasterReceiverBox1.getMin().squareDistance(shadowCasterReceiverBox2.getMin())<SQUARE_EPSILON
				&& shadowCasterReceiverBox1.getMax().squareDistance(shadowCasterReceiverBox2.getMax())<SQUARE_EPSILON;
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

		updateModelsRequiredUpdateFlag();

		this->models = models;
	}

	void FrustumShadowData::updateModelsRequiredUpdateFlag()
	{
		modelsRequiredUpdate = modelsMoved;

		if(modelsMoved)
		{
			modelsMovedCount++;
			if(modelsMovedCount % modelsInverseFrequencyUpdate!=0)
			{
				modelsRequiredUpdate = false;
			}
		}
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
		return shadowCasterReceiverBoxUpdated || modelsRequiredUpdate;
	}
}
