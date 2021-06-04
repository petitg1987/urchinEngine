#ifndef URCHINENGINE_SYNCFIXEDSIZEPOOL_H
#define URCHINENGINE_SYNCFIXEDSIZEPOOL_H

#include <mutex>

#include <utils/pool/FixedSizePool.h>

namespace urchin {

    template<class BaseType> class SyncFixedSizePool : public FixedSizePool<BaseType> {
        public:
            SyncFixedSizePool(const std::string&, unsigned int, unsigned int);
            ~SyncFixedSizePool() = default;

            void* allocate(unsigned int) override;
            void deallocate(BaseType*) override;

        private:
            std::mutex mutex;
    };

    #include "SyncFixedSizePool.inl"

}

#endif
