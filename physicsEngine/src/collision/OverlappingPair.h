#pragma once

#include <memory>

#include <body/model/AbstractBody.h>
#include <collision/narrowphase/algorithm/CollisionAlgorithm.h>
#include <collision/narrowphase/algorithm/CollisionAlgorithmSelector.h>

namespace urchin {

    /**
    * Overlapping pair representing two bodies potentially colliding
    */
    class OverlappingPair {
        public:
            OverlappingPair(AbstractBody&, AbstractBody&);
            OverlappingPair(AbstractBody&, AbstractBody&, uint_fast64_t);
            OverlappingPair(const OverlappingPair&);

            AbstractBody& getBody(unsigned int) const;
            AbstractBody& getBody1() const;
            AbstractBody& getBody2() const;

            static uint_fast64_t computeBodiesId(const AbstractBody&, const AbstractBody&);
            uint_fast64_t getBodiesId() const;

            void setCollisionAlgorithm(std::unique_ptr<CollisionAlgorithm, AlgorithmDeleter>);
            CollisionAlgorithm* getCollisionAlgorithm() const;

        private:
            AbstractBody& body1;
            AbstractBody& body2;
            uint_fast64_t bodiesId;

            std::unique_ptr<CollisionAlgorithm, AlgorithmDeleter> collisionAlgorithm;
    };

}
