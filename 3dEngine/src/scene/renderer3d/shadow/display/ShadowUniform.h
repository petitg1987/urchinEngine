#ifndef URCHINENGINE_SHADOWUNIFORM_H
#define URCHINENGINE_SHADOWUNIFORM_H

#include <vector>
#include "UrchinCommon.h"

#include "scene/renderer3d/shadow/data/ShadowData.h"
#include "scene/renderer3d/model/displayer/CustomUniform.h"

namespace urchin
{

	/**
	* Load custom uniform for shadow model displayer
	*/
	class ShadowUniform : public CustomUniform
	{
		public:
			ShadowUniform();
			~ShadowUniform();

			void setProjectionMatricesLocation(int);
			void setUniformData(const ShadowData *);

			void loadCustomUniforms();

		private:
			int mModelProjectionMatrixLoc;
			const ShadowData *shadowData;
	};

}

#endif
