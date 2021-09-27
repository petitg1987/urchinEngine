template<class T> void GridContainer<T>::addItem(T* item) {
    //to search on X axis
    std::int64_t xSortedMapKey = (((std::int64_t) item->getPosition().Y) << 32) + ((std::int64_t)item->getPosition().Z);
    auto xSortedItemsInsert = xSortedItems.insert(std::pair<std::int64_t, std::vector<T*>>(xSortedMapKey, {item}));
    if (!xSortedItemsInsert.second) { //not inserted
        VectorUtil::insertSorted((*xSortedItemsInsert.first).second, item, [](const T* item1, const T* item2) {return item1->getPosition().X < item2->getPosition().X;});
    }

    //to search on Y axis
    std::int64_t ySortedMapKey = (((std::int64_t) item->getPosition().X) << 32) + ((std::int64_t)item->getPosition().Z);
    auto ySortedItemsInsert = ySortedItems.insert(std::pair<std::int64_t, std::vector<T*>>(ySortedMapKey, {item}));
    if (!ySortedItemsInsert.second) { //not inserted
        VectorUtil::insertSorted((*ySortedItemsInsert.first).second, item, [](const T* item1, const T* item2) {return item1->getPosition().Y < item2->getPosition().Y;});
    }

    //to search on Z axis
    std::int64_t zSortedMapKey = (((std::int64_t) item->getPosition().X) << 32) + ((std::int64_t)item->getPosition().Y);
    auto zSortedItemsInsert = zSortedItems.insert(std::pair<std::int64_t, std::vector<T*>>(zSortedMapKey, {item}));
    if (!zSortedItemsInsert.second) { //not inserted
        VectorUtil::insertSorted((*zSortedItemsInsert.first).second, item, [](const T* item1, const T* item2) {return item1->getPosition().Z < item2->getPosition().Z;});
    }
}

template<class T> void GridContainer<T>::removeItem(T* item) {
    //TODO impl
}

template<class T> T* GridContainer<T>::findNeighbor(T* referenceItem, NeighborDirection neighborDirection) const {
    if (neighborDirection == NeighborDirection::X_POSITIVE) {
        std::int64_t key = (((std::int64_t) referenceItem->getPosition().Y) << 32) + ((std::int64_t)referenceItem->getPosition().Z);
        auto itFind = xSortedItems.find(key);
        if (itFind != xSortedItems.end()) {
            const std::vector<T*>& v = itFind->second;
            auto upper = std::upper_bound(v.begin(), v.end(), referenceItem, [](const T* item1, const T* item2) {return item1->getPosition().X < item2->getPosition().X;});
            if (upper != v.end()) {
                return *upper;
            }
        }

        return nullptr;
    } else {
        throw std::runtime_error("Unknown neighbor direction: " + std::to_string(neighborDirection));
    }
}