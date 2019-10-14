template<class TOctreeable> void OctreeableHelper<TOctreeable>::merge(std::vector<TOctreeable *> &targetOctreeables,
        const std::vector<TOctreeable *> &additionalOctreeables)
{
    std::for_each(targetOctreeables.begin(), targetOctreeables.end(), [](TOctreeable *o){o->setProcessed(true);});

    for(auto &additionalOctreeable : additionalOctreeables)
    {
        if(!additionalOctreeable->isProcessed())
        {
            targetOctreeables.emplace_back(additionalOctreeable);
        }
    }

    std::for_each(targetOctreeables.begin(), targetOctreeables.end(), [](TOctreeable *o){o->setProcessed(false);});
}
