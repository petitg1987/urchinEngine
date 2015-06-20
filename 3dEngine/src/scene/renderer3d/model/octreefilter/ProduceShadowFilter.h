#ifndef ENGINE_PRODUCESHADOWFILTER_H
#define ENGINE_PRODUCESHADOWFILTER_H

#include "scene/renderer3d/octree/filter/OctreeableFilter.h"
#include "scene/renderer3d/model/Model.h"

namespace urchin
{

	class ProduceShadowFilter : public OctreeableFilter<Model>
	{
		public:
			ProduceShadowFilter();
			~ProduceShadowFilter();

			bool isAccepted(const Model *const) const;
	};

}

#endif
