template<class T> AxisCompare<T>::AxisCompare(std::size_t axisIndex) :
        axisIndex(axisIndex) {

}

template<class T> bool AxisCompare<T>::operator() (const T& item1, const T& item2) const {
    return item1->getGridPosition()[axisIndex] < item2->getGridPosition()[axisIndex];
}

template<class T> void GridContainer<T>::addItem(std::shared_ptr<T> item) {
    for (std::size_t axisIndex = 0; axisIndex < 3; ++axisIndex) {
        std::int64_t key = buildKey(item->getGridPosition(), axisIndex);

        ItemSet<T> initialSet((AxisCompare<std::shared_ptr<T>>(axisIndex)));
        auto insertResult = axisSortedItems[axisIndex].insert(std::make_pair(key, initialSet));

        ItemSet<T>& setContainer = insertResult.first->second;
        setContainer.insert(item);
    }
}

template<class T> std::int64_t GridContainer<T>::buildKey(const Point3<int>& gridPosition, std::size_t excludeIndex) const {
    std::size_t index1 = (excludeIndex + 1) % 3;
    std::size_t index2 = (excludeIndex + 2) % 3;
    return (((std::int64_t)gridPosition[index1]) << 32) + ((std::int64_t)gridPosition[index2]);
}

template<class T> void GridContainer<T>::removeItem(const Point3<int>& gridPosition) {
    for (std::size_t axisIndex = 0; axisIndex < 3; ++axisIndex) {
        std::int64_t key = buildKey(gridPosition, axisIndex);

        auto itFindMap = axisSortedItems[axisIndex].find(key);
        if (itFindMap != axisSortedItems[axisIndex].end()) {
            auto& set = itFindMap->second;
            auto itFindSet = findInItemSet(set, gridPosition, axisIndex);
            if (itFindSet != set.end()) {
                set.erase(itFindSet);
            }

            if (set.empty()) {
                axisSortedItems[axisIndex].erase(itFindMap);
            }
        }
    }
}

template<class T> typename ItemSet<T>::iterator GridContainer<T>::findInItemSet(const ItemSet<T>& set, const Point3<int>& gridPosition, std::size_t axisIndex) const {
    auto lowerBound = std::lower_bound(set.begin(), set.end(), gridPosition, [&axisIndex](const std::shared_ptr<T>& item, const Point3<int>& pos) {
        return pos[axisIndex] > item->getGridPosition()[axisIndex];
    });
    if (lowerBound != set.end() && (*lowerBound)->getGridPosition()[axisIndex] == gridPosition[axisIndex]) {
        return lowerBound;
    }
    return set.end();
}

template<class T> bool GridContainer<T>::isItemExist(const Point3<int>& gridPosition) const {
    return getItem(gridPosition) != nullptr;
}

template<class T> T* GridContainer<T>::getItem(const Point3<int>& gridPosition) const {
    std::int64_t key = buildKey(gridPosition, 0);
    auto itFindMap = axisSortedItems[0].find(key);
    if (itFindMap != axisSortedItems[0].end()) {
        const auto& set = itFindMap->second;
        const auto itFind = findInItemSet(set, gridPosition, 0);
        if (itFind != set.end()) {
            return (*itFind).get();
        }
    }
    return nullptr;
}

/**
 * @tparam items [out] All items in grid container
 */
template<class T> void GridContainer<T>::getItems(std::vector<T*>& items) const {
    for (std::pair<std::int64_t, ItemSet<T>> element : axisSortedItems[0]) {
        for (auto it = element.second.begin(); it != element.second.end(); ++it) {
            items.push_back(it->get());
        }
    }
}

template<class T> std::shared_ptr<T> GridContainer<T>::findNeighbor(const Point3<int>& gridPosition, Axis axis, Direction direction) const {
    std::size_t axisIndex = (std::size_t)axis;
    const auto& sortedItems = axisSortedItems[axisIndex];

    std::int64_t key = buildKey(gridPosition, axisIndex);

    auto itFind = sortedItems.find(key);
    if (itFind != sortedItems.end()) {
        const auto& set = itFind->second;
        if (direction == Direction::POSITIVE) {
            auto upperBound = std::upper_bound(set.begin(), set.end(), gridPosition, [&axisIndex](const Point3<int>& pos, const std::shared_ptr<T>& item) {
                return pos[axisIndex] < item->getGridPosition()[axisIndex];
            });
            if (upperBound != set.end()) {
                return *upperBound;
            }
        } else if (direction == Direction::NEGATIVE) {
            auto lowerBound = std::lower_bound(set.begin(), set.end(), gridPosition, [&axisIndex](const std::shared_ptr<T>& item, const Point3<int>& pos) {
                return pos[axisIndex] > item->getGridPosition()[axisIndex];
            });
            if (lowerBound != set.begin()) {
                return *(--lowerBound);
            }
        } else {
            throw std::runtime_error("Unknown direction: " + std::to_string(direction));
        }
    }
    return nullptr;
}