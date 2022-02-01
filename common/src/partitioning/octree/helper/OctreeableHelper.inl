template<class T> void OctreeableHelper<T>::merge(std::vector<T*>& targetOctreeables, std::span<T* const> additionalOctreeables) {
    std::ranges::for_each(targetOctreeables, [](T* o){o->setProcessed(true);});

    for (auto& additionalOctreeable : additionalOctreeables) {
        if (!additionalOctreeable->isProcessed()) {
            targetOctreeables.emplace_back(additionalOctreeable);
        }
    }

    std::ranges::for_each(targetOctreeables, [](T* o){o->setProcessed(false);});
}
