#include <scene/renderer3d/model/culler/ModelOcclusionCuller.h>
#include <scene/renderer3d/util/OctreeRenderer.h>

namespace urchin {

    ModelOcclusionCuller::ModelOcclusionCuller() :
            modelOctreeManager(OctreeManager<Model>(ConfigService::instance().getFloatValue("model.octreeMinSize"))) {

    }

    void ModelOcclusionCuller::addModel(std::shared_ptr<Model> model) {
        modelOctreeManager.addOctreeable(std::move(model));
    }

    std::shared_ptr<Model> ModelOcclusionCuller::removeModel(Model* model) {
        return modelOctreeManager.removeOctreeable(model);
    }

    /**
     * @param modelsInFrustum [out] models in frustum
     */
    void ModelOcclusionCuller::getModelsInFrustum(const Frustum<float>& frustum, std::vector<Model*>& modelsInFrustum) const {
        assert(modelsInFrustum.empty());
        modelOctreeManager.getOctreeablesIn(frustum, modelsInFrustum);
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
