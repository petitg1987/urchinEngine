#ifndef URCHINENGINE_OCTREE_H
#define URCHINENGINE_OCTREE_H

#include <vector>
#include <limits>
#include <memory>
#include <algorithm>

#include "math/geometry/3d/object/AABBox.h"
#include "partitioning/octree/filter/OctreeableFilter.h"

namespace urchin {

    /**
    * Represents a node of the octree
    */
    template<class TOctreeable> class Octree {
        public:
            Octree(const Point3<float>&, const Vector3<float>&, float);
            ~Octree();

            const AABBox<float>& getAABBox() const;

            bool isLeaf() const;

            const std::vector<Octree<TOctreeable>*>& getChildren() const;

            const std::vector<TOctreeable*>& getOctreeables() const;
            void addOctreeable(TOctreeable*, bool addRef);
            void removeOctreeable(TOctreeable*, bool removeRef);

        private:
            std::vector<Octree*> children;
            std::vector<TOctreeable*> octreeables;

            AABBox<float> bbox;
            bool bIsLeaf;
    };

    #include "Octree.inl"

}

#endif
