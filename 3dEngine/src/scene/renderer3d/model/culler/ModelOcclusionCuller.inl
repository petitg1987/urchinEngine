/**
 * @param modelsInBox [out] models in OBBox
 */
template<class FILTER> void ModelOcclusionCuller::getModelsInOBBox(const OBBox<float>& box, std::vector<Model*>& modelsInBox, const FILTER& filter) const {
    assert(modelsInBox.empty());
    getNoCullModels(modelsInBox, filter);
    modelOctreeManager.getOctreeablesIn(box, modelsInBox, filter);
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
