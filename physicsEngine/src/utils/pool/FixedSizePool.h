#ifndef URCHINENGINE_FIXEDSIZEPOOL_H
#define URCHINENGINE_FIXEDSIZEPOOL_H

#include <cassert>
#include <sstream>
#include <string>
#include "UrchinCommon.h"

namespace urchin {

    /**
    * Pool which allocate a fixed size of memory defined in constructor argument. This pool offers high performance until the maximum
    * elements defined in constructor is not reached. Once maximum number of element reached, the performance strongly decrease.
    */
    template<class BaseType> class FixedSizePool {
        public:
            FixedSizePool(const std::string&, unsigned int, unsigned int);
            virtual ~FixedSizePool();

            virtual void* allocate(unsigned int);
            virtual void free(BaseType *ptr);

        private:
            void logPoolIsFull();

            std::string poolName;
            unsigned int maxElementSize;
            unsigned int maxElements;
            unsigned int freeCount; //number of free locations

            unsigned char* pool;
            void* firstFree;

            bool fullPoolLogged;
    };

    #include "FixedSizePool.inl"

}

#endif
