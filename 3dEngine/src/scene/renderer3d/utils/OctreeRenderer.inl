template<class T> void OctreeRenderer::drawOctree(const TargetRenderer *renderTarget, const OctreeManager<T> *octreeManager, const Matrix4<float> &projectionMatrix,
        const Matrix4<float> &viewMatrix) { //TODO move in OctreeManager ?
    auto leafOctrees = octreeManager->getAllLeafOctrees();

    std::vector<AABBox<float>> aabboxes;
    aabboxes.reserve(leafOctrees.size());

    for (const auto &leafOctree : leafOctrees) {
        aabboxes.push_back(leafOctree->getAABBox());
    }

    urchin::AABBoxModel aabboxModel(aabboxes);
    aabboxModel.onCameraProjectionUpdate(projectionMatrix);
    aabboxModel.display(renderTarget, viewMatrix);
}
