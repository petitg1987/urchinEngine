/**
 * @param models [out] models in convex object 3D
 */
template<class FILTER> void ModelOcclusionCuller::getModelsInConvexObject(const ConvexObject3D<float>& convexObject, std::vector<Model*>& models, bool strictFiltering, const FILTER& filter) const {
    assert(models.empty());
    getNoCullModels(models, filter);
    modelOctreeManager.getOctreeablesIn(convexObject, models, strictFiltering, filter);
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
