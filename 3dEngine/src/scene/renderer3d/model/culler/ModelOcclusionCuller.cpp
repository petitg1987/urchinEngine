#include <scene/renderer3d/model/culler/ModelOcclusionCuller.h>
#include <scene/renderer3d/util/OctreeRenderer.h>

namespace urchin {

    ModelOcclusionCuller::ModelOcclusionCuller() :
            modelOctreeManager(OctreeManager<Model>(ConfigService::instance().getFloatValue("model.octreeMinSize"))) {

    } //TODO handle flag update !

    void ModelOcclusionCuller::addModel(std::shared_ptr<Model> model) {
        if (model->getCullBehavior() == Model::CullBehavior::CULL) {
            modelOctreeManager.addOctreeable(std::move(model));
        } else if (model->getCullBehavior() == Model::CullBehavior::NO_CULL) {
            noCullModels.push_back(std::move(model));
        } else {
            throw std::runtime_error("Unknown cull behavior: " + std::to_string((int) model->getCullBehavior()));
        }
    }

    std::shared_ptr<Model> ModelOcclusionCuller::removeModel(Model* model) {
        if (model->getCullBehavior() == Model::CullBehavior::CULL) {
            return modelOctreeManager.removeOctreeable(model);
        } else if (model->getCullBehavior() == Model::CullBehavior::NO_CULL) {
            for (std::size_t i = 0; i < noCullModels.size(); ++i) {
                if (noCullModels[i].get() == model) {
                    std::shared_ptr<Model> modelPtr = std::move(noCullModels[i]);
                    VectorUtil::erase(noCullModels, i);
                    return modelPtr;
                }
            }
            return std::shared_ptr<Model>(nullptr);
        }
        throw std::runtime_error("Unknown cull behavior: " + std::to_string((int)model->getCullBehavior()));
    }

    /**
     * @param modelsInFrustum [out] models in frustum
     */
    void ModelOcclusionCuller::getModelsInFrustum(const Frustum<float>& frustum, std::vector<Model*>& modelsInFrustum) const {
        assert(modelsInFrustum.empty());
        auto acceptAllFilter = [](const Model* const){ return true; };
        addNoCullModels(modelsInFrustum, acceptAllFilter);
        modelOctreeManager.getOctreeablesIn(frustum, modelsInFrustum, acceptAllFilter);
    }

    std::unique_ptr<AABBoxModel> ModelOcclusionCuller::createDebugGeometries() {
        return OctreeRenderer::createOctreeModel(modelOctreeManager);
    }

    void ModelOcclusionCuller::refresh() {
        modelOctreeManager.refreshOctreeables();
    }

    void ModelOcclusionCuller::postRefresh() {
        modelOctreeManager.postRefreshOctreeables();
    }

}
