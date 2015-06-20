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
			FrustumShadowData(unsigned int);
			~FrustumShadowData();

			void setShadowCasterReceiverBox(const AABBox<float> &);
			const AABBox<float> &getShadowCasterReceiverBox() const;

			void setLightProjectionMatrix(const Matrix4<float> &);
			const Matrix4<float> &getLightProjectionMatrix() const;

			void setModels(const std::set<Model *> &);
			const std::set<Model *> &getModels() const;

		private:
			unsigned int frustumSplitIndex; //index of frustum split (0: frustum split nearest to eye)

			Matrix4<float> lightProjectionMatrix;
			AABBox<float> shadowCasterReceiverBox;

			std::set<Model *> models;
	};

}

#endif


