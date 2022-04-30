template<class Element, std::size_t Size> ProdConsCircular<Element, Size>::ProdConsCircular() :
        head(0),
        tail(0) {

}

template<class Element, std::size_t Size> bool ProdConsCircular<Element, Size>::push(Element item) {
    const std::size_t currentTail = tail.load(std::memory_order_relaxed);
    const std::size_t nextTail = increment(currentTail);

    if (nextTail != head.load(std::memory_order_acquire)) {
        array[currentTail] = std::move(item);
        tail.store(nextTail, std::memory_order_release);
        return true;
    }

    return false; //queue full
}

template<class Element, std::size_t Size> bool ProdConsCircular<Element, Size>::pop(Element& item) {
    const std::size_t currentHead = head.load(std::memory_order_relaxed);
    if (currentHead == tail.load(std::memory_order_acquire)) {
        return false; //queue empty
    }

    item = array[currentHead];
    head.store(increment(currentHead), std::memory_order_release);
    return true;
}

template<class Element, std::size_t Size> std::size_t ProdConsCircular<Element, Size>::increment(std::size_t index) const {
    return (index + 1) % Capacity;
}
