#pragma once

#include <limits>
#include <stdexcept>
#include <vector>
#include <memory>

#include <partitioning/octree/Octree.h>
#include <profiler/ScopeProfiler.h>
#include <pattern/observer/Observable.h>
#include <pattern/observer/Observer.h>
#include <util/StringUtil.h>

namespace urchin {

    template<class T> class OctreeManager final : public Observable, public Observer {
        public:
            explicit OctreeManager(float);
            OctreeManager(const OctreeManager&) = delete;
            OctreeManager& operator=(const OctreeManager&) = delete;
            ~OctreeManager() override;

            enum NotificationType {
                OCTREE_BUILT,
            };

            void notify(Observable*, int) override;

            bool addOctreeable(std::shared_ptr<T>);
            std::shared_ptr<T> removeOctreeable(T*);

            void updateMinSize(float);
            void refreshOctreeables();
            void postRefreshOctreeables();

            const Octree<T>& getMainOctree() const;
            std::vector<const Octree<T>*> getAllLeafOctrees() const;

            std::vector<std::shared_ptr<T>> getAllOctreeables() const;
            void getOctreeablesIn(const ConvexObject3D<float>&, std::vector<T*>&) const;
            template<class FILTER> void getOctreeablesIn(const ConvexObject3D<float>&, std::vector<T*>&, const FILTER&) const;

        private:
            void buildOctree(std::vector<std::shared_ptr<T>>&);
            bool resizeOctree(std::shared_ptr<T>);

            static constexpr unsigned int MAX_ERRORS_LOG = 20;

            float overflowSize;
            float minSize;
            std::unique_ptr<Octree<T>> mainOctree;

            std::vector<T*> movingOctreeables;
            mutable std::vector<Octree<T>*> browseNodes;

            unsigned int refreshModCount;
            unsigned int postRefreshModCount;
    };

    #include "OctreeManager.inl"

}
