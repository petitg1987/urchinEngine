/**
 * @param maxElementSize Size of element to store in pool. If there are several classes which extends 'BaseType', the biggest one
 *  should be provided
 * @param maxElement Maximum of elements which can be stored in the pool. If the maximum is exceed, a classical allocation
 *  (new/delete) will be performed.
 */
template<class BaseType> FixedSizePool<BaseType>::FixedSizePool(const std::string& poolName, unsigned int maxElementSize, unsigned int maxElements) :
        poolName(poolName),
        maxElementSize(maxElementSize),
        maxElements(maxElements),
        freeCount(maxElements),
        fullPoolLogged(false) {
    //create pool
    pool = static_cast<unsigned char*>(malloc(this->maxElementSize * this->maxElements));
    firstFree = pool;

    //initialize pool: each element contains address of next element and last one contains 0
    unsigned char* p = pool;
    int count = this->maxElements;
    while (--count) {
        *(void**)p = (p + this->maxElementSize);
        p += this->maxElementSize;
    }
    *(void**)p = 0;
}

template<class BaseType> FixedSizePool<BaseType>::~FixedSizePool() {
    if (freeCount != maxElements) //ensure that 'free' method has been called
    {
        Logger::instance()->logError("Fixed size pool '" + poolName + "' not correctly cleared. Free count: " + std::to_string(freeCount) + ", max elements: " + std::to_string(maxElements) + ".");
    }

    free(pool);
}

/**
 * @return Memory pointer which can be used to instantiate an element.
 */
template<class BaseType> void* FixedSizePool<BaseType>::allocate(unsigned int size) {
    if (size > maxElementSize) {
        throw std::runtime_error("Fixed size pool '" + poolName + "' cannot allocate " + std::to_string(size) + " bytes because max allowed allocation is " + std::to_string(maxElementSize) + " bytes");
    }

    if (freeCount != 0) { //pool is not full
        void* result = firstFree;
        firstFree = *(void**)firstFree;
        --freeCount;

        return result;
    }

    //pool is full: allocate new memory location
    logPoolIsFull();
    return operator new(maxElementSize);
}

/**
 * Call destructor of pointer and free location in the pool.
 * @param ptr Pointer to free
 */
template<class BaseType> void FixedSizePool<BaseType>::deallocate(BaseType* ptr) {
    if ((reinterpret_cast<unsigned char*>(ptr) >= pool && reinterpret_cast<unsigned char*>(ptr) < pool + maxElementSize*maxElements)) { //ptr is in the pool
        ptr->~BaseType();

        *(void**)ptr = firstFree;
        firstFree = ptr;
        ++freeCount;
    } else {
        delete ptr;
    }
}

template<class BaseType> void FixedSizePool<BaseType>::logPoolIsFull() {
    if (!fullPoolLogged) {
        std::stringstream logStream;
        logStream << "Pool is full of elements." << std::endl;
        logStream << " - Pool name: " << poolName << std::endl;
        logStream << " - Element size: " << maxElementSize << std::endl;
        logStream << " - Maximum elements: " << maxElements;
        Logger::instance()->logWarning(logStream.str());

        fullPoolLogged = true;
    }
}

