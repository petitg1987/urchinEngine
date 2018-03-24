#ifndef URCHINENGINE_OCTREE_H
#define URCHINENGINE_OCTREE_H

#include <vector>
#include <set>
#include <limits>
#include <memory>
#include <algorithm>
#include "UrchinCommon.h"

#include "scene/renderer3d/octree/filter/OctreeableFilter.h"
#include "utils/display/geometry/aabbox/AABBoxModel.h"

namespace urchin
{
	
	/**
	* Represents a node of the octree
	*/
	template<class TOctreeable> class Octree
	{
		public:
			Octree(const Point3<float> &, const Vector3<float> &, float);
			~Octree();
		
			const AABBox<float> &getAABBox() const;

			void addOctreeable(TOctreeable *, bool addRef);
			void removeOctreeable(TOctreeable *, bool removeRef);

			void getOctreeablesIn(std::set<TOctreeable *> &, const ConvexObject3D<float> &, const OctreeableFilter<TOctreeable> &) const;
			void getAllOctreeables(std::set<TOctreeable *> &) const;

			void getAllLeafOctrees(std::vector<const Octree<TOctreeable> *> &) const;

		private:
			std::vector<Octree *> children;
			std::vector<TOctreeable *> octreeables;
			
			AABBox<float> bbox;
			bool isLeaf;
	};

	#include "Octree.inl"

}

#endif
