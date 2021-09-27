template<class T>  AxisCompare<T>::AxisCompare(std::size_t axisIndex) :
        axisIndex(axisIndex) {

}

template<class T> bool AxisCompare<T>::operator() (const T& item1, const T& item2) const {
    return item1->getGridPosition()[axisIndex] < item2->getGridPosition()[axisIndex];
}

template<class T> void GridContainer<T>::addItem(T* item) {
    for (std::size_t axisIndex = 0; axisIndex < 3; ++axisIndex) {
        std::int64_t key = buildKey(item->getGridPosition(), axisIndex);

        std::set<T*, AxisCompare<T*>> initialSet((AxisCompare<T*>(axisIndex)));
        auto insertResult = axisSortedItems[axisIndex].insert(std::make_pair(key, initialSet));

        std::set<T*, AxisCompare<T*>>& setContainer = insertResult.first->second;
        setContainer.insert(item);
    }
}

template<class T> std::int64_t GridContainer<T>::buildKey(const Point3<int>& gridPosition, std::size_t excludeIndex) const {
    std::size_t index1 = (excludeIndex + 1) % 3;
    std::size_t index2 = (excludeIndex + 2) % 3;
    return (((std::int64_t)gridPosition[index1]) << 32) + ((std::int64_t)gridPosition[index2]);
}

template<class T> void GridContainer<T>::removeItem(T* item) {
    for (std::size_t axisIndex = 0; axisIndex < 3; ++axisIndex) {
        std::int64_t key = buildKey(item->getGridPosition(), axisIndex);

        auto itFind = axisSortedItems[axisIndex].find(key);
        if (itFind != axisSortedItems[axisIndex].end()) {
            itFind->second.erase(item);
            if (itFind->second.empty()) {
                axisSortedItems[axisIndex].erase(itFind);
            }
        }
    }
}

template<class T> T* GridContainer<T>::findNeighbor(const Point3<int>& gridPosition, Axis axis, Direction direction) const {
    std::size_t axisIndex = (std::size_t)axis;
    const auto& sortedItems = axisSortedItems[axisIndex];

    std::int64_t key = buildKey(gridPosition, axisIndex);

    auto itFind = sortedItems.find(key);
    if (itFind != sortedItems.end()) {
        const auto& set = itFind->second;
        if (direction == Direction::POSITIVE) {
            auto upper = std::upper_bound(set.begin(), set.end(), gridPosition, [&axisIndex](const Point3<int>& pos, T* item) {
                return pos[axisIndex] < item->getGridPosition()[axisIndex];
            });
            if (upper != set.end()) {
                return *upper;
            }
        } else if (direction == Direction::NEGATIVE) {
            auto lower = std::lower_bound(set.begin(), set.end(), gridPosition, [&axisIndex](T* item, const Point3<int>& pos) {
                return pos[axisIndex] > item->getGridPosition()[axisIndex];
            });
            if (lower != set.begin()) {
                return *(--lower);
            }
        } else {
            throw std::runtime_error("Unknown direction: " + std::to_string(direction));
        }
    }
    return nullptr;
}