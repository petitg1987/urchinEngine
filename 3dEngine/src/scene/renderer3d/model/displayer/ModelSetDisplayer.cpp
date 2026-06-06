#include <algorithm>
#include <stdexcept>
#include <ranges>

#include "scene/renderer3d/model/displayer/ModelSetDisplayer.h"
#include "graphics/render/shader/ShaderBuilder.h"

namespace urchin {

    ModelSetDisplayer::ModelSetDisplayer(DisplayMode displayMode) :
            isInitialized(false),
            displayMode(displayMode),
            customShaderVariable(nullptr),
            depthTestEnabled(true),
            depthWriteEnabled(true),
            enableFaceCull(true),
            enableLayerIndexDataInShader(false),
            renderTarget(nullptr) {

    }

    ModelSetDisplayer::~ModelSetDisplayer() {
        std::vector registeredModelsToRemove(registeredModels.begin(), registeredModels.end());
        for (Model* registeredModel : registeredModelsToRemove) {
            unregisterModel(registeredModel);
        }
        assert(exclusiveInstanceDisplayers.empty());
        assert(shareableInstanceDisplayers.empty());
    }

    void ModelSetDisplayer::initialize(RenderTarget& renderTarget) {
        if (isInitialized) {
            throw std::runtime_error("Model set displayer is already initialized.");
        }
        this->renderTarget = &renderTarget;

        assert(!vertexShaderName.empty());
        assert(!fragmentShaderName.empty());
        modelShader = ShaderBuilder::createShader(vertexShaderName, fragmentShaderName, std::move(shaderConstants), renderTarget.isTestMode());

        isInitialized = true;
    }

    void ModelSetDisplayer::setupShader(std::string vertexShaderName, std::string fragmentShaderName, std::unique_ptr<ShaderConstants> shaderConstants) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set custom shader once the model set displayer is initialized.");
        }

        this->vertexShaderName = std::move(vertexShaderName);
        this->fragmentShaderName = std::move(fragmentShaderName);
        this->shaderConstants = std::move(shaderConstants);
    }

    void ModelSetDisplayer::setupCustomShaderVariable(std::unique_ptr<CustomModelShaderVariable> customShaderVariable) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set custom shader variable once the model set displayer is initialized.");
        }
        this->customShaderVariable = std::move(customShaderVariable);
    }

    CustomModelShaderVariable* ModelSetDisplayer::getCustomShaderVariable() const {
        return this->customShaderVariable.get();
    }

    void ModelSetDisplayer::setupDepthOperations(bool depthTestEnabled, bool depthWriteEnabled) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set depth operations once the model set displayer is initialized.");
        }
        this->depthTestEnabled = depthTestEnabled;
        this->depthWriteEnabled = depthWriteEnabled;
    }

    void ModelSetDisplayer::setupFaceCull(bool enableFaceCull) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set face cull once the model set displayer is initialized.");
        }
        this->enableFaceCull = enableFaceCull;
    }

    void ModelSetDisplayer::setupBlendFunctions(const std::vector<BlendFunction>& blendFunctions) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set blend functions once the model set displayer is initialized.");
        }
        this->blendFunctions = blendFunctions;
    }

    void ModelSetDisplayer::setupLayerIndexDataInShader(bool enableLayerIndexDataInShader) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set layer index data in shader once the model set displayer is initialized.");
        }
        this->enableLayerIndexDataInShader = enableLayerIndexDataInShader;
    }

    void ModelSetDisplayer::setupMeshFilter(std::unique_ptr<MeshFilter> meshFilter) {
        this->meshFilter = std::move(meshFilter);
    }

    void ModelSetDisplayer::notify(Observable* observable, int notificationType) {
        if (auto model = dynamic_cast<Model*>(observable)) {
            ModelInstanceDisplayer* displayer = findModelInstanceDisplayer(*model);
            if (!displayer) {
                return;
            }

            bool canUpdateDisplayer = false;
            std::size_t newModelInstanceId = model->computeInstanceId(displayMode);
            if (newModelInstanceId != ModelDisplayable::INSTANCING_DENY_ID && displayer->getInstanceId() != ModelDisplayable::INSTANCING_DENY_ID) {
                if (displayer->getInstanceCount() <= 1 && !shareableInstanceDisplayers.contains(newModelInstanceId)) {
                    //case: displayer is not shared and there isn't other displayer matching the new instance ID
                    canUpdateDisplayer = true;

                    auto displayerNodeHandler = shareableInstanceDisplayers.extract(displayer->getInstanceId());
                    assert(displayerNodeHandler.mapped().get() == displayer);
                    displayerNodeHandler.mapped()->updateInstanceId(newModelInstanceId);
                    displayerNodeHandler.key() = newModelInstanceId;
                    shareableInstanceDisplayers.insert(std::move(displayerNodeHandler));
                } else if (newModelInstanceId == displayer->getInstanceId()) {
                    //case: update scale from 1.0 to 1.0, etc.
                    canUpdateDisplayer = true;
                }
            } else if (newModelInstanceId == ModelDisplayable::INSTANCING_DENY_ID && displayer->getInstanceId() == ModelDisplayable::INSTANCING_DENY_ID) {
                canUpdateDisplayer = true;
            }

            if (canUpdateDisplayer) {
                if (notificationType == Model::MESH_VERTICES_UPDATED) {
                    displayer->updateMeshVertices(model);
                } else if (notificationType == Model::MESH_UV_UPDATED) {
                    displayer->updateMeshUv(model);
                } else if (notificationType == Model::MATERIAL_UPDATED) {
                    displayer->updateMaterial(model);
                } else if (notificationType == Model::SCALE_UPDATED) {
                    displayer->updateScale();
                } else if (notificationType == Model::MODEL_PROPERTIES_UPDATED) {
                    displayer->updateModelProperties(model);
                }
            } else {
                //TODO what if addModelToDisplay is not re-call ?!!!!
                //The displayer will be replaced in the 'addModelToDisplay' method.
                //The displayer is not recreated directly here because we don't want to re-create it at each notification received for performance reason
                detachModelFromDisplayer(model, displayer);
            }
        }
    }

    ModelInstanceDisplayer* ModelSetDisplayer::findModelInstanceDisplayer(const Model& model) const {
        for (ModelInstanceDisplayer* modelInstanceDisplayer : model.getModelInstanceDisplayers()) {
            if (&modelInstanceDisplayer->getModelSetDisplayer() == this) {
                return modelInstanceDisplayer;
            }
        }
        return nullptr;
    }

    ModelInstanceDisplayer* ModelSetDisplayer::createOrUseDisplayerForModel(Model* model) {
        #ifdef URCHIN_DEBUG
            assert(!findModelInstanceDisplayer(*model));
        #endif

        std::size_t modelInstanceId = model->computeInstanceId(displayMode);

        if (modelInstanceId != ModelDisplayable::INSTANCING_DENY_ID) {
            const auto& itFind = shareableInstanceDisplayers.find(modelInstanceId);
            if (itFind != shareableInstanceDisplayers.end()) {
                //a matching model instance displayer has been found for the model
                itFind->second->addInstanceModel(*model);
                return itFind->second.get();
            }
        }

        auto modelInstanceDisplayer = std::make_unique<ModelInstanceDisplayer>(*this, displayMode, *renderTarget, *modelShader);
        modelInstanceDisplayer->setupCustomShaderVariable(customShaderVariable.get());
        modelInstanceDisplayer->setupDepthOperations(depthTestEnabled, depthWriteEnabled);
        modelInstanceDisplayer->setupBlendFunctions(blendFunctions);
        modelInstanceDisplayer->setupFaceCull(enableFaceCull);
        modelInstanceDisplayer->setupLayerIndexDataInShader(enableLayerIndexDataInShader);
        modelInstanceDisplayer->addInstanceModel(*model);
        modelInstanceDisplayer->initialize();
        if (modelInstanceId == ModelDisplayable::INSTANCING_DENY_ID) {
            const auto insertionResult = exclusiveInstanceDisplayers.try_emplace(model, std::move(modelInstanceDisplayer));
            assert(insertionResult.second);
            return insertionResult.first->second.get();
        } else {
            const auto insertionResult = shareableInstanceDisplayers.try_emplace(modelInstanceId, std::move(modelInstanceDisplayer));
            assert(insertionResult.second);
            return insertionResult.first->second.get();
        }
    }

    void ModelSetDisplayer::detachModelFromDisplayer(Model* model, ModelInstanceDisplayer* modelInstanceDisplayer) {
        modelInstanceDisplayer->removeInstanceModel(*model);

        if (exclusiveInstanceDisplayers.contains(model)) {
            exclusiveInstanceDisplayers.erase(model);
        } else if (modelInstanceDisplayer->getInstanceCount() == 0) {
            shareableInstanceDisplayers.erase(modelInstanceDisplayer->getInstanceId());
        }
    }

    void ModelSetDisplayer::observeModelUpdate(Model& model) {
        model.addObserver(this, Model::MESH_VERTICES_UPDATED);
        model.addObserver(this, Model::MESH_UV_UPDATED);
        model.addObserver(this, Model::MATERIAL_UPDATED);
        model.addObserver(this, Model::SCALE_UPDATED);
        model.addObserver(this, Model::MODEL_PROPERTIES_UPDATED);
    }

    void ModelSetDisplayer::unobserveModelUpdate(Model& model) {
        model.removeObserver(this, Model::SCALE_UPDATED);
        model.removeObserver(this, Model::MATERIAL_UPDATED);
        model.removeObserver(this, Model::MESH_UV_UPDATED);
        model.removeObserver(this, Model::MESH_VERTICES_UPDATED);
        model.removeObserver(this, Model::MODEL_PROPERTIES_UPDATED);
    }

    bool ModelSetDisplayer::registerModel(Model* model) {
        if (!isInitialized) {
            throw std::runtime_error("Model set displayer must be initialized before adding model");
        } else if (registeredModels.contains(model)) {
            return true;
        } else if (!model->getConstMeshes()) {
            return false;
        }

        registeredModels.emplace(model);
        observeModelUpdate(*model);

        createOrUseDisplayerForModel(model);
        return true;
    }

    void ModelSetDisplayer::unregisterModel(Model* model) {
        if (!model) {
            return;
        }

        ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*model);
        if (modelInstanceDisplayer) {
            detachModelFromDisplayer(model, modelInstanceDisplayer);
        }

        unobserveModelUpdate(*model);
        registeredModels.erase(model);

        std::erase(modelsToDisplay, model);
    }

    void ModelSetDisplayer::addModelToDisplay(Model* modelToDisplay, std::bitset<8> layersMask) {
        bool modelRegistered = registerModel(modelToDisplay);

        if (modelRegistered && (!meshFilter || meshFilter->isAccepted(*modelToDisplay))) {
            ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*modelToDisplay);
            if (!modelInstanceDisplayer) {
                //No model displayer found. Possible causes:
                // - Displayer was not anymore valid due to changes applied on the model
                // - The displayer has been purged because it has not been used for a long time
                modelInstanceDisplayer = createOrUseDisplayerForModel(modelToDisplay);
            }

            modelInstanceDisplayer->updateLayersMask(modelInstanceDisplayer->getLayersMask() | layersMask);

            this->modelsToDisplay.push_back(modelToDisplay);
        }
    }

    void ModelSetDisplayer::replaceModelsToDisplay(std::span<Model* const> modelsToDisplay) {
        resetModelsToDisplay();
        for (Model* modelToDisplay : modelsToDisplay) {
            addModelToDisplay(modelToDisplay);
        }
    }

    void ModelSetDisplayer::resetModelsToDisplay() {
        this->modelsToDisplay.clear();

        for (const std::unique_ptr<ModelInstanceDisplayer>& modelInstanceDisplayer : std::views::values(exclusiveInstanceDisplayers)) {
            modelInstanceDisplayer->updateLayersMask(std::bitset<8>());
        }
        for (const std::unique_ptr<ModelInstanceDisplayer>& modelInstanceDisplayer : std::views::values(shareableInstanceDisplayers)) {
            modelInstanceDisplayer->updateLayersMask(std::bitset<8>());
        }
    }

    std::span<Model* const> ModelSetDisplayer::getModelsToDisplay() const {
        return modelsToDisplay;
    }

    bool ModelSetDisplayer::isDisplayerExist(const Model& model) const {
        return findModelInstanceDisplayer(model) != nullptr;
    }

    void ModelSetDisplayer::prepareRendering(unsigned int renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        ScopeProfiler sp(Profiler::graphic(), "modelPreRender");

        if (!isInitialized) {
            throw std::runtime_error("Model set displayer must be initialized before call display");
        }

        activeInstanceDisplayers.clear();
        for (const Model* model: modelsToDisplay) {
            ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*model);
            assert(modelInstanceDisplayer);
            if (activeInstanceDisplayers.insert(modelInstanceDisplayer)) {
                modelInstanceDisplayer->resetRenderingModels();
            }
            modelInstanceDisplayer->registerRenderingModel(*model);
        }
        for (ModelInstanceDisplayer* activeModelDisplayer: activeInstanceDisplayers) {
            activeModelDisplayer->prepareRendering(renderingOrder, projectionViewMatrix, meshFilter.get());
        }

        purgeUnusedInstanceDisplayers();
    }

    void ModelSetDisplayer::prepareRendering(unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix, const ModelSortFunction& modelSorter, const void* userData) {
        ScopeProfiler sp(Profiler::graphic(), "modelPreRender");

        if (!isInitialized) {
            throw std::runtime_error("Model set displayer must be initialized before call display");
        }

        std::ranges::sort(modelsToDisplay, [&modelSorter, &userData](const Model* model1, const Model* model2) {
            return modelSorter(model1, model2, userData);
        });
        for (const Model* model: modelsToDisplay) {
            ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*model);
            modelInstanceDisplayer->resetRenderingModels();
            modelInstanceDisplayer->registerRenderingModel(*model);

            renderingOrder++;
            modelInstanceDisplayer->prepareRendering(renderingOrder, projectionViewMatrix, meshFilter.get());
        }
    }

    void ModelSetDisplayer::purgeUnusedInstanceDisplayers() {
        constexpr double OLD_DISPLAYERS_TIME_MS = 30'000.0;
        auto purgeOldInstanceDisplayers = [](auto& instanceDisplayersMap) {
            auto currentTime = std::chrono::steady_clock::now();
            for (auto it = instanceDisplayersMap.begin(); it != instanceDisplayersMap.end();) {
                auto unusedTimeInMs = (double)std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - it->second->getLastRenderingTime()).count();
                if (unusedTimeInMs >= OLD_DISPLAYERS_TIME_MS) {
                    it = instanceDisplayersMap.erase(it);
                } else {
                    ++it;
                }
            }
        };
        purgeOldInstanceDisplayers(exclusiveInstanceDisplayers);
        purgeOldInstanceDisplayers(shareableInstanceDisplayers);
    }

    void ModelSetDisplayer::drawBBox(GeometryContainer& geometryContainer) const {
        for (const auto& model : modelsToDisplay) {
            findModelInstanceDisplayer(*model)->drawBBox(geometryContainer);
        }
    }

    void ModelSetDisplayer::drawBaseBones(GeometryContainer& geometryContainer) const {
        for (const auto& model : modelsToDisplay) {
            if (model->getConstMeshes()) {
                findModelInstanceDisplayer(*model)->drawBaseBones(geometryContainer, meshFilter.get());
            }
        }
    }

}
