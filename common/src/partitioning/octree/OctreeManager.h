#pragma once

#include <limits>
#include <stdexcept>
#include <vector>
#include <memory>

#include <partitioning/octree/Octree.h>
#include <profiler/ScopeProfiler.h>
#include <pattern/observer/Observable.h>
#include <pattern/observer/Observer.h>
#include <partitioning/octree/filter/OctreeableFilter.h>
#include <partitioning/octree/filter/AcceptAllFilter.h>

namespace urchin {

    template<class TOctreeable> class OctreeManager : public Observable, public Observer {
        public:
            explicit OctreeManager(float);
            ~OctreeManager() override;

            enum NotificationType {
                OCTREE_BUILT,
            };

            void notify(Observable*, int) override;

            void addOctreeable(std::shared_ptr<TOctreeable>);
            std::shared_ptr<TOctreeable> removeOctreeable(TOctreeable*);

            void updateMinSize(float);
            void refreshOctreeables();
            void postRefreshOctreeables();

            const Octree<TOctreeable>& getMainOctree() const;
            std::vector<const Octree<TOctreeable>*> getAllLeafOctrees() const;

            std::vector<std::shared_ptr<TOctreeable>> getAllOctreeables() const;
            void getOctreeablesIn(const ConvexObject3D<float>&, std::vector<TOctreeable*>&) const;
            void getOctreeablesIn(const ConvexObject3D<float>&, std::vector<TOctreeable*>&, const OctreeableFilter<TOctreeable>&) const;

        private:
            void buildOctree(std::vector<std::shared_ptr<TOctreeable>>&);
            bool resizeOctree(std::shared_ptr<TOctreeable>);

            float overflowSize;
            float minSize;
            Octree<TOctreeable>* mainOctree;

            std::vector<TOctreeable*> movingOctreeables;
            mutable std::vector<Octree<TOctreeable>*> browseNodes;

            unsigned int refreshModCount, postRefreshModCount;
    };

    #include "OctreeManager.inl"

}
