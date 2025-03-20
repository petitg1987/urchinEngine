#include <scene/renderer3d/model/culler/ModelOcclusionCuller.h>
#include <scene/renderer3d/util/OctreeRenderer.h>

namespace urchin {

    ModelOcclusionCuller::ModelOcclusionCuller() :
            modelOctreeManager(OctreeManager<Model>(ConfigService::instance().getFloatValue("model.octreeMinSize"))) {

    }

    void ModelOcclusionCuller::notify(Observable* observable, int notificationType) {
        if (auto* model = dynamic_cast<Model*>(observable)) {
            if (notificationType == Model::CULL_BEHAVIOR_UPDATED) {
                if (model->getCullBehavior() == Model::CullBehavior::CULL) {
                    std::shared_ptr<Model> modelPtr = removeModel(model, Model::CullBehavior::NO_CULL);
                    addModel(modelPtr);
                } else if (model->getCullBehavior() == Model::CullBehavior::NO_CULL) {
                    std::shared_ptr<Model> modelPtr = removeModel(model, Model::CullBehavior::CULL);
                    addModel(modelPtr);
                } else {
                    throw std::runtime_error("Unknown cull behavior: " + std::to_string((int)model->getCullBehavior()));
                }
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
        return removeModel(model, model->getCullBehavior());
    }

    std::shared_ptr<Model> ModelOcclusionCuller::removeModel(Model* model, Model::CullBehavior cullBehavior) {
        std::shared_ptr<Model> modelPtr = nullptr;
        if (cullBehavior == Model::CullBehavior::CULL) {
            modelPtr = modelOctreeManager.removeOctreeable(model);
        } else if (cullBehavior == Model::CullBehavior::NO_CULL) {
            for (std::size_t i = 0; i < noCullModels.size(); ++i) {
                if (noCullModels[i].get() == model) {
                    modelPtr = std::move(noCullModels[i]);
                    VectorUtil::erase(noCullModels, i);
                    break;
                }
            }
        } else {
            throw std::runtime_error("Unknown cull behavior: " + std::to_string((int)cullBehavior));
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
        getNoCullModels(modelsInFrustum, acceptAllFilter);
        modelOctreeManager.getOctreeablesIn(frustum, modelsInFrustum, acceptAllFilter);
    }

    std::vector<std::shared_ptr<Model>> ModelOcclusionCuller::getAllModels() const {
        std::vector<std::shared_ptr<Model>> allModels = modelOctreeManager.getAllOctreeables();
        allModels.insert(allModels.end(), noCullModels.begin(), noCullModels.end());
        return allModels;
    }

    std::unique_ptr<AABBoxModel> ModelOcclusionCuller::createDebugGeometries() const {
        return OctreeRenderer::createOctreeModel(modelOctreeManager);
    }

    void ModelOcclusionCuller::refresh() {
        modelOctreeManager.refreshOctreeables();
    }

    void ModelOcclusionCuller::postRefresh() {
        modelOctreeManager.postRefreshOctreeables();
    }

}
