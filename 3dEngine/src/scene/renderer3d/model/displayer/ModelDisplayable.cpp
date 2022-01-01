#include <scene/renderer3d/model/displayer/ModelDisplayable.h>
#include <scene/renderer3d/model/displayer/ModelInstanceDisplayer.h>

namespace urchin {

    ModelDisplayable::~ModelDisplayable() {
        assert(modelInstanceDisplayers.empty()); //can not remove a model when displayer still use it
    }

    void ModelDisplayable::attachModelInstanceDisplayer(ModelInstanceDisplayer& modelInstanceDisplayer) {
        auto* model = static_cast<Model*>(this);
        modelInstanceDisplayer.addInstanceModel(*model);

        modelInstanceDisplayers.push_back(&modelInstanceDisplayer);
    }

    void ModelDisplayable::detachModelInstanceDisplayer(ModelInstanceDisplayer& modelInstanceDisplayer) {
        auto* model = static_cast<Model*>(this);
        modelInstanceDisplayer.removeInstanceModel(*model);

        std::size_t erasedCount = std::erase_if(modelInstanceDisplayers, [&modelInstanceDisplayer](const ModelInstanceDisplayer* mid){ return mid == &modelInstanceDisplayer; });
        if (erasedCount != 1) {
            throw std::runtime_error("Removing the model displayer from model fail: " + model->getConstMeshes()->getId());
        }
    }

    const std::vector<ModelInstanceDisplayer*>& ModelDisplayable::getModelInstanceDisplayers() const {
        return modelInstanceDisplayers;
    }

    std::size_t ModelDisplayable::computeInstanceId(DisplayMode displayMode) const {
        const auto* model = static_cast<const Model*>(this);

        if (model->isMeshUpdated()) {
            //no instancing on models animated or where mesh is manually updated
            return INSTANCING_DENY_ID;
        }

        std::size_t instanceHash = 0;
        HashUtil::combine(instanceHash, model->getConstMeshes()->getId());

        if (displayMode == DisplayMode::DEFAULT_MODE) {
            bool hasUvScaling = false;
            for (unsigned int meshIndex = 0; meshIndex < model->getMeshes()->getNumberMeshes(); ++meshIndex) {
                Mesh& mesh = model->getMeshes()->getMesh(meshIndex);
                HashUtil::combine(instanceHash, mesh.getMaterial().getId());
                hasUvScaling |= mesh.getMaterial().getUvScale().hasScaling();
            }
            if (hasUvScaling) {
                //UV are not part of the shader instance variables. Therefore, different scaling cannot be part of the same model instancing.
                HashUtil::combine(instanceHash, model->getTransform().getScale().X, model->getTransform().getScale().Y, model->getTransform().getScale().Z);
            }
            return instanceHash;
        } else if (displayMode == DisplayMode::DEPTH_ONLY_MODE) {
            return instanceHash;
        }

        throw std::runtime_error("Unknown display mode: " + std::to_string((int)displayMode));
    }

}

