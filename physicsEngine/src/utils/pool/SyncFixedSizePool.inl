template<class BaseType> SyncFixedSizePool<BaseType>::SyncFixedSizePool(const std::string& poolName, unsigned int maxElementSize, unsigned int maxElements) :
        FixedSizePool<BaseType>(poolName, maxElementSize, maxElements) {

}

template<class BaseType> void* SyncFixedSizePool<BaseType>::allocate(unsigned int size) {
    std::scoped_lock lock(mutex);

    return FixedSizePool<BaseType>::allocate(size);
}

template<class BaseType> void SyncFixedSizePool<BaseType>::deallocate(BaseType* ptr) {
    std::scoped_lock lock(mutex);

    FixedSizePool<BaseType>::deallocate(ptr);
}
