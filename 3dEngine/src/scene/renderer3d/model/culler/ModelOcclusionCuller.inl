/**
 * @param modelsInBox [out] models in OBBox
 */
template<class FILTER> void ModelOcclusionCuller::getModelsInOBBox(const OBBox<float>& box, std::vector<Model*>& modelsInBox, const FILTER& filter) const {
    assert(modelsInBox.empty());
    modelOctreeManager.getOctreeablesIn(box, modelsInBox, filter);
}
