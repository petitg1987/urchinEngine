template<class T> std::unique_ptr<AABBoxModel> OctreeRenderer::createOctreeModel(const OctreeManager<T>& octreeManager) {
    auto leafOctrees = octreeManager.getAllLeafOctrees();

    std::vector<AABBox<float>> aabboxes;
    aabboxes.reserve(leafOctrees.size());

    for (const auto& leafOctree : leafOctrees) {
        aabboxes.push_back(leafOctree->getAABBox());
    }

    return std::make_unique<AABBoxModel>(aabboxes);
}
