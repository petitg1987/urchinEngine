#ifndef URCHINENGINE_OCTREEABLEFILTER_H
#define URCHINENGINE_OCTREEABLEFILTER_H

namespace urchin
{

	template<class TOctreeable> class OctreeableFilter
	{
		public:
			virtual ~OctreeableFilter() = default;

			virtual bool isAccepted(const TOctreeable *, const ConvexObject3D<float> &) const = 0;
	};

}

#endif
