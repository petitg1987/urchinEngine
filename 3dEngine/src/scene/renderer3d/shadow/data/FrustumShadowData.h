#ifndef URCHINENGINE_FRUSTUMSHADOWDATA_H
#define URCHINENGINE_FRUSTUMSHADOWDATA_H

#include "UrchinCommon.h"

#include "scene/renderer3d/model/Model.h"

namespace urchin
{

	/**
	* Shadow execution data for a light and a splitted frustum
	*/
	class FrustumShadowData
	{
		public:
			explicit FrustumShadowData(unsigned int);

			void updateShadowCasterReceiverBox(const AABBox<float> &, bool);
			const AABBox<float> &getShadowCasterReceiverBox() const;
			const Matrix4<float> &getLightProjectionMatrix() const;
			bool isShadowCasterReceiverBoxUpdated() const;

			void updateModels(const std::unordered_set<Model *> &);
			const std::unordered_set<Model *> &getModels() const;

			bool needShadowMapUpdate() const;

		private:
			bool areIdenticalAABBox(const AABBox<float> &, const AABBox<float> &) const;

			unsigned int frustumSplitIndex; //index of frustum split (0: frustum split nearest to eye)
            bool isFarFrustumSplit;

			Matrix4<float> lightProjectionMatrix;
			AABBox<float> shadowCasterReceiverBox;
			bool shadowCasterReceiverBoxUpdated;

			std::unordered_set<Model *> models;
			bool modelsRequireUpdate;
	};

}

#endif


