#ifndef URCHINENGINE_OCTREEABLEFILTER_H
#define URCHINENGINE_OCTREEABLEFILTER_H

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
