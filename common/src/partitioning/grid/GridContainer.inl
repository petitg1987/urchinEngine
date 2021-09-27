template<class T> void GridContainer<T>::addItem(T* item) {
    //to search on X axis
    std::uint64_t xSortedMapKey = (((std::uint64_t) item->getPosition().Y) << 32) + ((std::uint64_t)item->getPosition().Z);
    auto xSortedItemsInsert = xSortedItems.insert(std::pair<std::int64_t, std::vector<T*>>(xSortedMapKey, {item}));
    if (!xSortedItemsInsert.second) { //not inserted
        VectorUtil::insertSorted((*xSortedItemsInsert.first).second, item, [](const T* item1, const T* item2) {return item1->getPosition().X < item2->getPosition().X;});
    }

    //to search on Y axis
    std::uint64_t ySortedMapKey = (((std::uint64_t) item->getPosition().X) << 32) + ((std::uint64_t)item->getPosition().Z);
    auto ySortedItemsInsert = ySortedItems.insert(std::pair<std::int64_t, std::vector<T*>>(ySortedMapKey, {item}));
    if (!ySortedItemsInsert.second) { //not inserted
        VectorUtil::insertSorted((*ySortedItemsInsert.first).second, item, [](const T* item1, const T* item2) {return item1->getPosition().Y < item2->getPosition().Y;});
    }

    //to search on Z axis
    std::uint64_t zSortedMapKey = (((std::uint64_t) item->getPosition().X) << 32) + ((std::uint64_t)item->getPosition().Y);
    auto zSortedItemsInsert = zSortedItems.insert(std::pair<std::int64_t, std::vector<T*>>(zSortedMapKey, {item}));
    if (!zSortedItemsInsert.second) { //not inserted
        VectorUtil::insertSorted((*zSortedItemsInsert.first).second, item, [](const T* item1, const T* item2) {return item1->getPosition().Z < item2->getPosition().Z;});
    }
}

template<class T> void GridContainer<T>::removeItem(T* item) {
    //TODO impl
}

template<class T> T* GridContainer<T>::findNeighbor(T* referenceItem, NeighborDirection neighborDirection) const {
    //TODO impl
}