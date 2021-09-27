template<class T>  AxisCompare<T>::AxisCompare(std::size_t axisIndex) :
        axisIndex(axisIndex) {

}

template<class T> bool AxisCompare<T>::operator() (const T& item1, const T& item2) const {
    return item1->getPosition()[axisIndex] < item2->getPosition()[axisIndex];
}

template<class T> void GridContainer<T>::addItem(T* item) {
    for (std::size_t axisIndex = 0; axisIndex < 3; ++axisIndex) {
        std::int64_t key = buildKey(item, axisIndex);

        std::set<T*, AxisCompare<T*>> initialSet((AxisCompare<T*>(axisIndex)));
        auto insertResult = axisSortedItems[axisIndex].insert(std::make_pair(key, initialSet));

        std::set<T*, AxisCompare<T*>>& setContainer = insertResult.first->second;
        setContainer.insert(item);
    }
}

template<class T> std::int64_t GridContainer<T>::buildKey(T* item, std::size_t excludeIndex) const {
    std::size_t index1 = (excludeIndex + 1) % 3;
    std::size_t index2 = (excludeIndex + 2) % 3;
    return (((std::int64_t) item->getPosition()[index1]) << 32) + ((std::int64_t)item->getPosition()[index2]);
}

template<class T> void GridContainer<T>::removeItem(T* item) {
    for (std::size_t axisIndex = 0; axisIndex < 3; ++axisIndex) {
        std::int64_t key = buildKey(item, axisIndex);

        auto itFind = axisSortedItems[axisIndex].find(key);
        if (itFind != axisSortedItems[axisIndex].end()) {
            itFind->second.erase(item);
            if(itFind->second.empty()) {
                axisSortedItems[axisIndex].erase(itFind);
            }
        }
    }
}

template<class T> T* GridContainer<T>::findNeighbor(T* referenceItem, Axis axis, Direction direction) const {
    std::size_t searchAxisIndex = (std::size_t)axis;
    const auto& sortedItems = axisSortedItems[searchAxisIndex];

    std::int64_t key = buildKey(referenceItem, searchAxisIndex);

    auto itFind = sortedItems.find(key);
    if (itFind != sortedItems.end()) {
        const auto& set = itFind->second;
        if (direction == Direction::POSITIVE) {
            auto upper = set.upper_bound(referenceItem);
            if (upper != set.end()) {
                return *upper;
            }
        } else if (direction == Direction::NEGATIVE) {
            auto lower = set.lower_bound(referenceItem);
            if (lower != set.begin()) {
                return *(--lower);
            }
        } else {
            throw std::runtime_error("Unknown direction: " + std::to_string(direction));
        }
    }
    return nullptr;
}