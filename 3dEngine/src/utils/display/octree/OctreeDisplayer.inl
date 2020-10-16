
template<class T> OctreeDisplayer<T>::OctreeDisplayer(const OctreeManager<T> *octreeManager) :
        octreeManager(octreeManager) {

}

template<class T> void OctreeDisplayer<T>::drawOctree(const Matrix4<float> &projectionMatrix, const Matrix4<float> &viewMatrix) const {
    auto leafOctrees = octreeManager->getAllLeafOctrees();

    std::vector<AABBox<float>> aabboxes;
    aabboxes.reserve(leafOctrees.size());

    for(const auto &leafOctree : leafOctrees) {
        aabboxes.push_back(leafOctree->getAABBox());
    }

    urchin::AABBoxModel aabboxModel(aabboxes);
    aabboxModel.onCameraProjectionUpdate(projectionMatrix);
    aabboxModel.display(viewMatrix);
}
