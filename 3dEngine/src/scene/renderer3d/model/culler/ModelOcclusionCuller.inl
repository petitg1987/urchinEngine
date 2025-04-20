/**
 * @param modelsInFrustum [out] models in frustum
 */
template<class FILTER> void ModelOcclusionCuller::getModelsInFrustum(const Frustum<float>& frustum, std::vector<Model*>& modelsInFrustum, bool strictFiltering, const FILTER& filter) const {
    assert(modelsInFrustum.empty());
    getNoCullModels(modelsInFrustum, filter);
    modelOctreeManager.getOctreeablesIn(frustum, modelsInFrustum, strictFiltering, filter);
}

/**
 * @param modelsInBox [out] models in OBBox
 */
template<class FILTER> void ModelOcclusionCuller::getModelsInOBBox(const OBBox<float>& box, std::vector<Model*>& modelsInBox, bool strictFiltering, const FILTER& filter) const {
    assert(modelsInBox.empty());
    getNoCullModels(modelsInBox, filter);
    modelOctreeManager.getOctreeablesIn(box, modelsInBox, strictFiltering, filter);
}

/**
 * @param models [out] models matching filter
 */
template<class FILTER> void ModelOcclusionCuller::getNoCullModels(std::vector<Model*>& models, const FILTER& filter) const {
    for (const std::shared_ptr<Model>& noCullModel : noCullModels) {
        if (noCullModel->isVisible() && filter(noCullModel.get())) {
            models.push_back(noCullModel.get());
        }
    }
}
