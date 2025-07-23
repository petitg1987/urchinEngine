#pragma once

#include <memory>

#include "body/model/AbstractBody.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmSelector.h"

namespace urchin {

    /**
    * Overlapping pair representing two bodies potentially colliding
    */
    class OverlappingPair {
        public:
            OverlappingPair(std::shared_ptr<AbstractBody>, std::shared_ptr<AbstractBody>);
            OverlappingPair(std::shared_ptr<AbstractBody>, std::shared_ptr<AbstractBody>, uint_fast64_t);
            OverlappingPair(const OverlappingPair&);
            OverlappingPair& operator=(const OverlappingPair&) = delete;
            ~OverlappingPair() = default;

            AbstractBody& getBody(unsigned int) const;
            AbstractBody& getBody1() const;
            const std::shared_ptr<AbstractBody>& getBody1Ptr() const;
            AbstractBody& getBody2() const;
            const std::shared_ptr<AbstractBody>& getBody2Ptr() const;

            static uint_fast64_t computeBodiesId(const AbstractBody&, const AbstractBody&);
            uint_fast64_t getBodiesId() const;

            void setCollisionAlgorithm(std::unique_ptr<CollisionAlgorithm, AlgorithmDeleter>);
            CollisionAlgorithm* getCollisionAlgorithm() const;

        private:
            //keep the ownership on the bodies via std::shared_ptr because bodies could be removed from physics thread but still present in the copied overlapping pairs
            std::shared_ptr<AbstractBody> body1;
            std::shared_ptr<AbstractBody> body2;
            uint_fast64_t bodiesId;

            std::unique_ptr<CollisionAlgorithm, AlgorithmDeleter> collisionAlgorithm;
    };

}
