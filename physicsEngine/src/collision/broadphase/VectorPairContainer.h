#pragma once

#include <vector>

#include "collision/OverlappingPair.h"
#include "collision/broadphase/PairContainer.h"

namespace urchin {

    /**
    * Overlapping pair manager using a std::vector. Vectors have very high performance
    * to looping over. Add and remove operations have also good performance when number
    * of elements are limited (< 1000) due to little cache miss.
    */
    class VectorPairContainer : public PairContainer {
        public:
            ~VectorPairContainer() override = default;

            void addOverlappingPair(std::shared_ptr<AbstractBody>, std::shared_ptr<AbstractBody>) override;
            void removeOverlappingPair(AbstractBody&, AbstractBody&) override;
            void removeOverlappingPairs(AbstractBody&) override;

            const std::vector<std::unique_ptr<OverlappingPair>>& getOverlappingPairs() const override;
            void retrieveCopyOverlappingPairs(std::vector<OverlappingPair>&) const override;

        protected:
            std::vector<std::unique_ptr<OverlappingPair>> overlappingPairs;
    };

}
