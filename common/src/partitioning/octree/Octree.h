#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <cassert>

#include <util/VectorUtil.h>
#include <math/geometry/3d/object/AABBox.h>

namespace urchin {

    /**
    * Represents a node of the octree
    */
    template<class T> class Octree {
        public:
            Octree(const Point3<float>&, const Vector3<float>&, float);
            Octree(const Octree&) = delete;
            Octree& operator=(const Octree&) = delete;
            ~Octree();

            const AABBox<float>& getAABBox() const;

            bool isLeaf() const;

            const std::vector<std::unique_ptr<Octree<T>>>& getChildren() const;

            const std::vector<std::shared_ptr<T>>& getOctreeables() const;
            void addOctreeable(std::shared_ptr<T>, bool addRef);
            std::shared_ptr<T> removeOctreeable(T*, bool removeRef);

        private:
            std::vector<std::unique_ptr<Octree>> children;
            std::vector<std::shared_ptr<T>> octreeables;

            AABBox<float> bbox;
            bool bIsLeaf;
    };

    #include "Octree.inl"

}
