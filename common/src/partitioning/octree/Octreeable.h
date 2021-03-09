#ifndef URCHINENGINE_OCTREEABLE_H
#define URCHINENGINE_OCTREEABLE_H

#include <vector>
#include <stdexcept>

#include "Octree.h"
#include "pattern/observer/Observable.h"

namespace urchin {

    /**
    * Virtual class that can be inserted into an octree
    */
    template<class TOctreeable> class Octreeable : public Observable {
        public:
            Octreeable();
            Octreeable(const Octreeable&);
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

            const std::vector<Octree<TOctreeable>*>& getRefOctree() const;
            void addRefOctree(Octree<TOctreeable>*);
            void removeRefOctree(Octree<TOctreeable>*);

            virtual const AABBox<float>& getAABBox() const = 0;
            virtual const Transform<float>& getTransform() const = 0;

        private:
            std::vector<Octree<TOctreeable>*> refOctree;

            bool bIsMovingInOctree;
            bool bIsVisible;
            bool bIsProcessed;
    };

    #include "Octreeable.inl"

}

#endif
