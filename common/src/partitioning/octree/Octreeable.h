#pragma once

#include <vector>
#include <stdexcept>

#include <partitioning/octree/Octree.h>
#include <pattern/observer/Observable.h>

namespace urchin {

    /**
    * Virtual class that can be inserted into an octree
    */
    template<class T> class Octreeable : public Observable {
        public:
            Octreeable();
            Octreeable(const Octreeable<T>&);
            ~Octreeable() override;

            enum NotificationType {
                MOVE,
                MAX_NOTIFICATION_TYPE
            };

            void notifyOctreeableMove();
            void onMoveDone();
            bool isMovingInOctree() const;

            void setVisible(bool);
            bool isVisible() const;

            void setProcessed(bool);
            bool isProcessed() const;

            const std::vector<Octree<T>*>& getRefOctree() const;
            void addRefOctree(Octree<T>*);
            void removeRefOctree(Octree<T>*);

            virtual const AABBox<float>& getAABBox() const = 0;
            virtual const Transform<float>& getTransform() const = 0;

        private:
            std::vector<Octree<T>*> refOctree;

            bool bIsMovingInOctree;
            bool bIsVisible;
            bool bIsProcessed;
    };

    #include "Octreeable.inl"

}
