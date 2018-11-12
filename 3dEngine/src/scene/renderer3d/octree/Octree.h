#ifndef URCHINENGINE_OCTREE_H
#define URCHINENGINE_OCTREE_H

#include <vector>
#include <unordered_set>
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

			bool isLeaf() const;

			const std::vector<Octree<TOctreeable> *> &getChildren() const;

            const std::vector<TOctreeable *> &getOctreeables() const;
			void addOctreeable(TOctreeable *, bool addRef);
			void removeOctreeable(TOctreeable *, bool removeRef);

		private:
			std::vector<Octree *> children;
			std::vector<TOctreeable *> octreeables;

			AABBox<float> bbox;
			bool bIsLeaf;
	};

	#include "Octree.inl"

}

#endif
