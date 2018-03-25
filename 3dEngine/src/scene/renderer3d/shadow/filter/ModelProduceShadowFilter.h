#ifndef URCHINENGINE_MODELPRODUCESHADOWFILTER
#define URCHINENGINE_MODELPRODUCESHADOWFILTER

#include "UrchinCommon.h"

#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/octree/filter/OctreeableFilter.h"

namespace urchin
{

	class ModelProduceShadowFilter : public OctreeableFilter<Model>
	{
		public:
			~ModelProduceShadowFilter() override = default;

			bool isAccepted(const Model *const, const ConvexObject3D<float> &) const;
	};
}

#endif
