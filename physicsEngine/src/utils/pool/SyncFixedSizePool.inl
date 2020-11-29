template<class BaseType> SyncFixedSizePool<BaseType>::SyncFixedSizePool(const std::string& poolName, unsigned int maxElementSize, unsigned int maxElements) :
        FixedSizePool<BaseType>(poolName, maxElementSize, maxElements) {

}

template<class BaseType> void* SyncFixedSizePool<BaseType>::allocate(unsigned int size) {
    std::lock_guard<std::mutex> lock(mutex);

    return FixedSizePool<BaseType>::allocate(size);
}

template<class BaseType> void SyncFixedSizePool<BaseType>::free(BaseType* ptr) {
    std::lock_guard<std::mutex> lock(mutex);

    FixedSizePool<BaseType>::free(ptr);
}
