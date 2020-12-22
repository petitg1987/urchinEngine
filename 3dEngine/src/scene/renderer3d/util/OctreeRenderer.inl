template<class T> void OctreeRenderer::drawOctree(const OctreeManager<T>* octreeManager, const RenderTarget* renderTarget,
        const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) {
    auto leafOctrees = octreeManager->getAllLeafOctrees();

    std::vector<AABBox<float>> aabboxes;
    aabboxes.reserve(leafOctrees.size());

    for (const auto& leafOctree : leafOctrees) {
        aabboxes.push_back(leafOctree->getAABBox());
    }

    urchin::AABBoxModel aabboxModel(aabboxes);
    aabboxModel.onCameraProjectionUpdate(projectionMatrix);
    aabboxModel.display(renderTarget, viewMatrix);
}
