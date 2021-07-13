template<class T> void OctreeableHelper<T>::merge(std::vector<T*>& targetOctreeables,
        const std::vector<T*>& additionalOctreeables) {
    std::for_each(targetOctreeables.begin(), targetOctreeables.end(), [](T* o){o->setProcessed(true);});

    for (auto& additionalOctreeable : additionalOctreeables) {
        if (!additionalOctreeable->isProcessed()) {
            targetOctreeables.emplace_back(additionalOctreeable);
        }
    }

    std::for_each(targetOctreeables.begin(), targetOctreeables.end(), [](T* o){o->setProcessed(false);});
}
