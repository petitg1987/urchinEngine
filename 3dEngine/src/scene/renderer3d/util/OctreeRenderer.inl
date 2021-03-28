template<class T> void OctreeRenderer::drawOctree(const OctreeManager<T>* octreeManager, const std::shared_ptr<RenderTarget>& renderTarget,
        const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) {
    auto leafOctrees = octreeManager->getAllLeafOctrees();

    std::vector<AABBox<float>> aabboxes;
    aabboxes.reserve(leafOctrees.size());

    for (const auto& leafOctree : leafOctrees) {
        aabboxes.push_back(leafOctree->getAABBox());
    }

    urchin::AABBoxModel aabboxModel(aabboxes);
    aabboxModel.initialize(renderTarget);
    aabboxModel.onCameraProjectionUpdate(projectionMatrix);
    aabboxModel.prepareRendering(viewMatrix);
}
