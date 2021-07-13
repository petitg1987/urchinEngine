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

    template<class T> class OctreeManager : public Observable, public Observer {
        public:
            explicit OctreeManager(float);
            ~OctreeManager() override;

            enum NotificationType {
                OCTREE_BUILT,
            };

            void notify(Observable*, int) override;

            void addOctreeable(std::shared_ptr<T>);
            std::shared_ptr<T> removeOctreeable(T*);

            void updateMinSize(float);
            void refreshOctreeables();
            void postRefreshOctreeables();

            const Octree<T>& getMainOctree() const;
            std::vector<const Octree<T>*> getAllLeafOctrees() const;

            std::vector<std::shared_ptr<T>> getAllOctreeables() const;
            void getOctreeablesIn(const ConvexObject3D<float>&, std::vector<T*>&) const;
            void getOctreeablesIn(const ConvexObject3D<float>&, std::vector<T*>&, const OctreeableFilter<T>&) const;

        private:
            void buildOctree(std::vector<std::shared_ptr<T>>&);
            bool resizeOctree(std::shared_ptr<T>);

            float overflowSize;
            float minSize;
            Octree<T>* mainOctree;

            std::vector<T*> movingOctreeables;
            mutable std::vector<Octree<T>*> browseNodes;

            unsigned int refreshModCount, postRefreshModCount;
    };

    #include "OctreeManager.inl"

}
