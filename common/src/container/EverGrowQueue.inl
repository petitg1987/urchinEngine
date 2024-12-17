template<class T> EverGrowQueue<T>::EverGrowQueue(int initialCapacity, int chunkSize) :
        data(initialCapacity),
        capacity(initialCapacity),
        size(0),
        frontIndex(0),
        backIndex(capacity - 1),
        chunkSize(chunkSize) {
    data.resize(capacity);
}

template<class T> void EverGrowQueue<T>::pushFront(const T& value) {
    if (size == capacity) {
        expandCapacity();
    }

    frontIndex = (frontIndex - 1 + capacity) % capacity;
    data[frontIndex] = value;
    ++size;
}

template<class T> void EverGrowQueue<T>::pushBack(const T& value) {
    if (size == capacity) {
        expandCapacity();
    }

    backIndex = (backIndex + 1) % capacity;
    data[backIndex] = value;
    ++size;
}

template<class T> T EverGrowQueue<T>::popFront() {
    #ifdef URCHIN_DEBUG
        assert(size != 0);
    #endif

    T value = data[frontIndex];
    frontIndex = (frontIndex + 1) % capacity;
    --size;
    return value;
}

template<class T> T EverGrowQueue<T>::popBack() {
    #ifdef URCHIN_DEBUG
        assert(size != 0);
    #endif

    T value = data[backIndex];
    backIndex = (backIndex - 1 + capacity) % capacity;
    --size;
    return value;
}

template<class T> bool EverGrowQueue<T>::isEmpty() const {
    return size == 0;
}

template<class T> int EverGrowQueue<T>::getSize() const {
    return size;
}

template<class T> int EverGrowQueue<T>::getCapacity() const {
    return capacity;
}

template<class T> void EverGrowQueue<T>::expandCapacity() {
    std::vector<T> newData(capacity + chunkSize);
    for (std::size_t i = 0; i < size; ++i) {
        newData[i] = data[(frontIndex + i) % capacity];
    }
    data = std::move(newData);
    capacity += chunkSize;
    frontIndex = 0;
    backIndex = size - 1;
}