#include "FrustumShadowData.h"

namespace urchin
{

	FrustumShadowData::FrustumShadowData(unsigned int frustumSplitIndex) :
			frustumSplitIndex(frustumSplitIndex),
            isFarFrustumSplit(false),
			shadowCasterReceiverBoxUpdated(false),
            modelsRequireUpdate(false)
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
	void FrustumShadowData::updateModels(const std::vector<Model *> &models)
	{
        modelsRequireUpdate = false;

        if(models != this->models)
        {
            modelsRequireUpdate = true;
        }else
        {
            for (auto model : models)
            {
                if(model->isMovingInOctree() || model->isAnimate())
                {
                    modelsRequireUpdate = true;
                    break;
                }
            }
        }

		this->models = models;
	}

	/**
	 * @return Models visible from light in frustum split
	 */
	const std::vector<Model *> &FrustumShadowData::getModels() const
	{
		return models;
	}

	bool FrustumShadowData::needShadowMapUpdate() const
	{
		return shadowCasterReceiverBoxUpdated || modelsRequireUpdate;
	}
}
