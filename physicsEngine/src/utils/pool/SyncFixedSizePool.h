#pragma once

#include <mutex>

#include "utils/pool/FixedSizePool.h"

namespace urchin {

    template<class BaseType> class SyncFixedSizePool final : public FixedSizePool<BaseType> {
        public:
            SyncFixedSizePool(const std::string&, unsigned int, unsigned int);
            ~SyncFixedSizePool() override = default;

            void* allocate(unsigned int) override;
            void deallocate(BaseType*) override;

        private:
            std::mutex mutex;
    };

    #include "SyncFixedSizePool.inl"

}
