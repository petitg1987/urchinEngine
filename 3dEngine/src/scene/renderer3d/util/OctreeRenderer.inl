template<class T> std::unique_ptr<AABBoxModel> OctreeRenderer::createOctreeModel(const OctreeManager<T>& octreeManager, RenderTarget& renderTarget,
        const Matrix4<float>& projectionMatrix) {
    auto leafOctrees = octreeManager.getAllLeafOctrees();

    std::vector<AABBox<float>> aabboxes;
    aabboxes.reserve(leafOctrees.size());

    for (const auto& leafOctree : leafOctrees) {
        aabboxes.push_back(leafOctree->getAABBox());
    }

    auto aabboxModel = std::make_unique<AABBoxModel>(aabboxes);
    aabboxModel->initialize(renderTarget);
    aabboxModel->onCameraProjectionUpdate(projectionMatrix);

    return aabboxModel;
}
