template<class T> void OctreeDisplayer::drawOctree(const OctreeManager<T> *octreeManager, const Matrix4<float> &projectionMatrix,
        const Matrix4<float> &viewMatrix) {
    auto leafOctrees = octreeManager->getAllLeafOctrees();

    std::vector<AABBox<float>> aabboxes;
    aabboxes.reserve(leafOctrees.size());

    for (const auto &leafOctree : leafOctrees) {
        aabboxes.push_back(leafOctree->getAABBox());
    }

    urchin::AABBoxModel aabboxModel(aabboxes);
    aabboxModel.onCameraProjectionUpdate(projectionMatrix);
    aabboxModel.display(viewMatrix);
}
