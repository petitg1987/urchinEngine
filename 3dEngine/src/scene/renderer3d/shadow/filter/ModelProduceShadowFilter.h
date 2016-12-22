#ifndef ENGINE_MODELPRODUCESHADOWFILTER
#define ENGINE_MODELPRODUCESHADOWFILTER

#include "UrchinCommon.h"

#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/octree/filter/OctreeableFilter.h"

namespace urchin
{

	class ModelProduceShadowFilter : public OctreeableFilter<Model>
	{
		public:
			ModelProduceShadowFilter();
			~ModelProduceShadowFilter();

			bool isAccepted(const Model *const) const;
	};
}

#endif
