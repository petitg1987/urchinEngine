#ifndef TESTENGINESFML_SYNCFIXEDSIZEPOOL_H
#define TESTENGINESFML_SYNCFIXEDSIZEPOOL_H

#include <mutex>

#include "utils/pool/FixedSizePool.h"

namespace urchin
{

    template<class BaseType> class SyncFixedSizePool : public FixedSizePool<BaseType>
    {
        public:
            SyncFixedSizePool(const std::string &, unsigned int, unsigned int);
            ~SyncFixedSizePool() = default;

            void* allocate(unsigned int);
            void free(BaseType *ptr);

        private:
            std::mutex mutex;
    };

    #include "SyncFixedSizePool.inl"

}

#endif
