#include "scene/renderer3d/model/displayer/ModelDisplayable.h"
#include "scene/renderer3d/model/displayer/ModelInstanceDisplayer.h"

namespace urchin {

    ModelDisplayable::~ModelDisplayable() {
        assert(modelInstanceDisplayers.empty()); //can not remove a model when displayer still use it
    }

    void ModelDisplayable::attachModelInstanceDisplayer(ModelInstanceDisplayer& modelInstanceDisplayer) {
        modelInstanceDisplayers.push_back(&modelInstanceDisplayer);
    }

    void ModelDisplayable::detachModelInstanceDisplayer(ModelInstanceDisplayer& modelInstanceDisplayer) {
        std::size_t erasedCount = std::erase(modelInstanceDisplayers, &modelInstanceDisplayer);
        if (erasedCount != 1) {
            const auto* model = static_cast<const Model*>(this);
            throw std::runtime_error("Removing the model displayer from model fail: " + model->getConstMeshes()->getId());
        }
    }

    std::span<ModelInstanceDisplayer* const> ModelDisplayable::getModelInstanceDisplayers() const {
        return modelInstanceDisplayers;
    }

    std::size_t ModelDisplayable::computeInstanceId(DisplayMode displayMode) const {
        const auto* model = static_cast<const Model*>(this);

        if (!model->getMeshes() || model->isOriginalVerticesOrUvUpdated() || displayMode == DisplayMode::DEFAULT_NO_INSTANCING_MODE) {
            //Currently, the instance hash is computed on the getConstMeshes() and not on the getMesh() for the vertices/UV.
            //Therefore, it is not possible to identify when instancing is required on mesh where the vertices/UV have been updated (manually or by an animation).
            return INSTANCING_DENY_ID; //no instancing
        }

        std::size_t instanceHash = model->getConstMeshes()->retrieveHashId();
        if (displayMode == DisplayMode::DEFAULT_MODE) {
            HashUtil::combine(instanceHash, model->getLightMask());
            bool hasUvScaling = false;
            for (unsigned int meshIndex = 0; meshIndex < model->getMeshes()->getNumMeshes(); ++meshIndex) {
                const Material& material = model->getMeshes()->getMesh(meshIndex).getMaterial();
                HashUtil::combine(instanceHash, material.retrieveHashId());
                hasUvScaling = hasUvScaling || material.getUvScale().hasScaling();
            }
            if (hasUvScaling) {
                //UV are not part of the shader instance variables. Therefore, different scaling cannot be part of the same model instancing.
                float scaleHashId = model->getTransform().getScale().X * 10000.0f + model->getTransform().getScale().Y * 100.0f + model->getTransform().getScale().Z;
                HashUtil::combine(instanceHash, scaleHashId);
            }
            return instanceHash;
        } else if (displayMode == DisplayMode::DEPTH_ONLY_MODE) {
            return instanceHash;
        }

        throw std::runtime_error("Unknown display mode: " + std::to_string((int)displayMode));
    }

}

