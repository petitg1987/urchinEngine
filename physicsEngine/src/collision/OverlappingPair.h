#ifndef URCHINENGINE_OVERLAPPINGPAIR_H
#define URCHINENGINE_OVERLAPPINGPAIR_H

#include <memory>

#include <body/model/AbstractBody.h>
#include <collision/narrowphase/algorithm/CollisionAlgorithm.h>

namespace urchin {

    /**
    * Overlapping pair representing two bodies potentially colliding
    */
    class OverlappingPair {
        public:
            OverlappingPair(AbstractBody*, AbstractBody*);
            OverlappingPair(AbstractBody*, AbstractBody*, uint_fast64_t);
            OverlappingPair(const OverlappingPair&) = default;

            AbstractBody* getBody(unsigned int) const;
            AbstractBody* getBody1() const;
            AbstractBody* getBody2() const;

            static uint_fast64_t computeBodiesId(const AbstractBody*, const AbstractBody*);
            uint_fast64_t getBodiesId() const;

            void setCollisionAlgorithm(std::shared_ptr<CollisionAlgorithm>);
            std::shared_ptr<CollisionAlgorithm> getCollisionAlgorithm() const;

        private:
            AbstractBody* body1;
            AbstractBody* body2;
            uint_fast64_t bodiesId;

            std::shared_ptr<CollisionAlgorithm> collisionAlgorithm;
    };

}

#endif
