#ifndef ENGINE_OCTREEABLEFILTER_H
#define ENGINE_OCTREEABLEFILTER_H

#include "UrchinCommon.h"

namespace urchin
{

	template<class TOctreeable> class OctreeableFilter
	{
		public:
			OctreeableFilter();
			virtual ~OctreeableFilter();

			virtual bool isAccepted(const TOctreeable *const) const = 0;
	};

	#include "OctreeableFilter.inl"

}

#endif
