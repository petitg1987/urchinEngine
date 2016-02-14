#ifndef ENGINE_FRUSTUMSHADOWDATA_H
#define ENGINE_FRUSTUMSHADOWDATA_H

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
			FrustumShadowData(unsigned int, float);
			~FrustumShadowData();

			void updateShadowCasterReceiverBox(const AABBox<float> &, bool);
			const AABBox<float> &getShadowCasterReceiverBox() const;
			const Matrix4<float> &getLightProjectionMatrix() const;
			bool isShadowCasterReceiverBoxUpdated() const;

			void updateModels(const std::set<Model *> &);
			const std::set<Model *> &getModels() const;
			bool isModelsMoved() const;

			bool needShadowMapUpdate() const;

		private:
			bool areIdenticalAABBox(const AABBox<float> &, const AABBox<float> &) const;
			void updateModelsRequiredUpdateFlag();

			unsigned int frustumSplitIndex; //index of frustum split (0: frustum split nearest to eye)

			Matrix4<float> lightProjectionMatrix;
			AABBox<float> shadowCasterReceiverBox;
			bool shadowCasterReceiverBoxUpdated;

			std::set<Model *> models;
			bool modelsMoved;

			const unsigned int modelsInverseFrequencyUpdate;
			unsigned int modelsMovedCount;
			bool modelsRequiredUpdate;
	};

}

#endif


