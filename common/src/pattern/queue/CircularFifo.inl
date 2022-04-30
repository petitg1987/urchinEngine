template<class T, std::size_t Size> CircularFifo<T, Size>::CircularFifo() :
        head(0),
        tail(0) {

}

template<class T, std::size_t Size> bool CircularFifo<T, Size>::push(T item) {
    const std::size_t currentTail = tail.load(std::memory_order_relaxed);
    const std::size_t nextTail = increment(currentTail);

    if (nextTail != head.load(std::memory_order_acquire)) {
        array[currentTail] = std::move(item);
        tail.store(nextTail, std::memory_order_release);
        return true;
    }

    return false; //queue full
}

template<class T, std::size_t Size> bool CircularFifo<T, Size>::pop(T& item) {
    const std::size_t currentHead = head.load(std::memory_order_relaxed);
    if (currentHead == tail.load(std::memory_order_acquire)) {
        return false; //queue empty
    }

    item = array[currentHead];
    head.store(increment(currentHead), std::memory_order_release);
    return true;
}

template<class T, std::size_t Size> std::size_t CircularFifo<T, Size>::increment(std::size_t index) const {
    return (index + 1) % Capacity;
}
