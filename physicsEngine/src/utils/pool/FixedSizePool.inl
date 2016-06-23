/**
 * @param maxElementSize Size of element to store in pool. If there are several classes which extends 'BaseType', the biggest one
 *  should be provided
 * @param maxElement Maximum of elements which can be stored in the pool. If the maximum is exceed, a classical allocation
 *  (new/delete) will be performed and performance will be bad.
 */
template<class BaseType> FixedSizePool<BaseType>::FixedSizePool(unsigned int maxElementSize, unsigned int maxElements) :
		maxElementSize(maxElementSize),
		maxElements(maxElements),
		freeCount(maxElements)
{
	//create pool
	pool = static_cast<unsigned char *>(operator new(this->maxElementSize * this->maxElements));
	firstFree = pool;
	
	//initialize pool: each element contains address of next element and last one contains 0
	unsigned char *p = pool;
	int count = this->maxElements;
	while (--count)
	{
		*(void**)p = (p + this->maxElementSize);
		p += this->maxElementSize;
	}
	*(void**)p = 0;
}

template<class BaseType> inline FixedSizePool<BaseType>::~FixedSizePool()
{
	#ifdef _DEBUG
		assert(freeCount==maxElements); //ensure that destructors have been called
	#endif
	
	delete pool;
}

/**
 * @return Memory pointer which can be used to instantiate an element.
 */
template<class BaseType> inline void* FixedSizePool<BaseType>::allocate(unsigned int elementSize)
{
	assert(elementSize<=maxElementSize);
	
	if(freeCount!=0)
	{ //pool is not full
		void* result = firstFree;
		firstFree = *(void**)firstFree;
		--freeCount;
		return result;
	}
	
	//pool is full: allocate new memory location
	#ifdef _DEBUG
		logPoolIsFull();
	#endif
	return operator new(elementSize);
}

/**
 * Call destructor of pointer and free location in the pool.
 * @param ptr Pointer to free
 */
template<class BaseType> inline void FixedSizePool<BaseType>::free(BaseType *ptr)
{
	if (((unsigned char*)ptr >= pool && (unsigned char*)ptr < pool + maxElementSize*maxElements))
	{ //ptr is in the pool
		ptr->~BaseType();
		
		*(void**)ptr = firstFree;
		firstFree = ptr;
		++freeCount;
	}else
	{
		delete ptr;
	}
}
	
#ifdef _DEBUG
	template<class BaseType> void FixedSizePool<BaseType>::logPoolIsFull() const
	{
		Logger::setLogger(new FileLogger());
		Logger::logger()<<Logger::prefix(Logger::LOG_WARNING);
		Logger::logger()<<"Pool is full of elements."<<"\n";
		Logger::logger()<<" - Element size: "<<maxElementSize<<"\n";
		Logger::logger()<<" - Maximum elements: "<<maxElements<<"\n\n";
		Logger::setLogger(nullptr);
	}
#endif

