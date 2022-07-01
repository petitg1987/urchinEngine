#include <scene/renderer3d/model/culler/ModelOcclusionCuller.h>
#include <scene/renderer3d/util/OctreeRenderer.h>

namespace urchin {

    ModelOcclusionCuller::ModelOcclusionCuller() :
            modelOctreeManager(OctreeManager<Model>(ConfigService::instance().getFloatValue("model.octreeMinSize"))) {

    } //TODO add test !

    void ModelOcclusionCuller::notify(Observable* observable, int notificationType) {
        if (auto* model = dynamic_cast<Model*>(observable)) {
            if (notificationType == Model::CULL_BEHAVIOR_UPDATED) {
                std::shared_ptr<Model> modelPtr = removeModel(model);
                addModel(modelPtr);
            }
        }
    }

    void ModelOcclusionCuller::addModel(std::shared_ptr<Model> model) {
        model->addObserver(this, Model::CULL_BEHAVIOR_UPDATED);

        if (model->getCullBehavior() == Model::CullBehavior::CULL) {
            modelOctreeManager.addOctreeable(std::move(model));
        } else if (model->getCullBehavior() == Model::CullBehavior::NO_CULL) {
            noCullModels.push_back(std::move(model));
        } else {
            throw std::runtime_error("Unknown cull behavior: " + std::to_string((int)model->getCullBehavior()));
        }
    }

    std::shared_ptr<Model> ModelOcclusionCuller::removeModel(Model* model) {
        std::shared_ptr<Model> modelPtr = nullptr;
        if (model->getCullBehavior() == Model::CullBehavior::CULL) {
            modelPtr = modelOctreeManager.removeOctreeable(model);
        } else if (model->getCullBehavior() == Model::CullBehavior::NO_CULL) {
            for (std::size_t i = 0; i < noCullModels.size(); ++i) {
                if (noCullModels[i].get() == model) {
                    modelPtr = std::move(noCullModels[i]);
                    VectorUtil::erase(noCullModels, i);
                    break;
                }
            }
        } else {
            throw std::runtime_error("Unknown cull behavior: " + std::to_string((int) model->getCullBehavior()));
        }

        if (modelPtr) {
            modelPtr->removeObserver(this, Model::CULL_BEHAVIOR_UPDATED);
        }
        return modelPtr;
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
