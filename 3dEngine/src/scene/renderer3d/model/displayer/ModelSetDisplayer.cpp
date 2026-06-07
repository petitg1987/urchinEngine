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
            renderTarget(nullptr),
            registeredModels({}) {

    }

    ModelSetDisplayer::~ModelSetDisplayer() {
        for (auto it = registeredModels.begin(); it != registeredModels.end();) {
            it = unregisterModel(it);
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
            #ifdef URCHIN_DEBUG
                assert(registeredModels.contains(model));
            #endif

            ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*model);

            bool canUpdateDisplayer = false;
            std::size_t newModelInstanceId = model->computeInstanceId(displayMode);
            if (newModelInstanceId != ModelDisplayable::INSTANCING_DENY_ID && modelInstanceDisplayer->getInstanceId() != ModelDisplayable::INSTANCING_DENY_ID) {
                if (modelInstanceDisplayer->getInstanceCount() <= 1 && !shareableInstanceDisplayers.contains(newModelInstanceId)) {
                    //case: displayer is not shared and there isn't other displayer matching the new instance ID
                    canUpdateDisplayer = true;

                    auto displayerNodeHandler = shareableInstanceDisplayers.extract(modelInstanceDisplayer->getInstanceId());
                    assert(displayerNodeHandler.mapped().get() == modelInstanceDisplayer);
                    displayerNodeHandler.mapped()->updateInstanceId(newModelInstanceId);
                    displayerNodeHandler.key() = newModelInstanceId;
                    shareableInstanceDisplayers.insert(std::move(displayerNodeHandler));
                } else if (newModelInstanceId == modelInstanceDisplayer->getInstanceId()) {
                    //case: update scale from 1.0 to 1.0, etc.
                    canUpdateDisplayer = true;
                }
            } else if (newModelInstanceId == ModelDisplayable::INSTANCING_DENY_ID && modelInstanceDisplayer->getInstanceId() == ModelDisplayable::INSTANCING_DENY_ID) {
                canUpdateDisplayer = true;
            }

            if (canUpdateDisplayer) {
                if (notificationType == Model::MESH_VERTICES_UPDATED) {
                    modelInstanceDisplayer->updateMeshVertices(model);
                } else if (notificationType == Model::MESH_UV_UPDATED) {
                    modelInstanceDisplayer->updateMeshUv(model);
                } else if (notificationType == Model::MATERIAL_UPDATED) {
                    modelInstanceDisplayer->updateMaterial(model);
                } else if (notificationType == Model::SCALE_UPDATED) {
                    modelInstanceDisplayer->updateScale();
                } else if (notificationType == Model::MODEL_PROPERTIES_UPDATED) {
                    modelInstanceDisplayer->updateModelProperties(model);
                }
            } else {
                detachModelFromDisplayer(model, modelInstanceDisplayer);
                createOrUseDisplayerForModel(model);
            }
        }
    }

    ModelInstanceDisplayer* ModelSetDisplayer::findModelInstanceDisplayer(const Model& model) const {
        for (ModelInstanceDisplayer* modelInstanceDisplayer : model.getModelInstanceDisplayers()) {
            if (&modelInstanceDisplayer->getModelSetDisplayer() == this) {
                return modelInstanceDisplayer;
            }
        }
        throw std::runtime_error("Missing model instance displayer for model: " + model.getName());
    }

    ModelInstanceDisplayer* ModelSetDisplayer::createOrUseDisplayerForModel(Model* model) {
        #ifdef URCHIN_DEBUG
            assert(std::ranges::none_of(model->getModelInstanceDisplayers(), [&](const ModelInstanceDisplayer* displayer) {
                return &displayer->getModelSetDisplayer() == this;
            }));
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
        modelInstanceDisplayer->setupLayersMask(std::bitset<8>(0));
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

        registeredModels.emplace(model, std::chrono::steady_clock::now());
        observeModelUpdate(*model);

        createOrUseDisplayerForModel(model);
        return true;
    }

    void ModelSetDisplayer::unregisterModel(Model* model) {
        if (model) {
            RegisteredModelIt itRegisteredModel = registeredModels.find(model);
            if (itRegisteredModel != registeredModels.end()) {
                unregisterModel(itRegisteredModel);
            }
        }
    }

    ModelSetDisplayer::RegisteredModelIt ModelSetDisplayer::unregisterModel(RegisteredModelIt itRegisteredModel) {
        Model *model = itRegisteredModel->first;
        ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*model);
        detachModelFromDisplayer(model, modelInstanceDisplayer);

        unobserveModelUpdate(*model);
        std::erase(modelsToDisplay, model);
        return registeredModels.erase(itRegisteredModel);
    }

    void ModelSetDisplayer::purgeUnusedRegisteredModels() {
        constexpr double OLD_MODELS_TIME_MS = 30'000.0;
        auto currentTime = std::chrono::steady_clock::now();
        for (auto it = registeredModels.begin(); it != registeredModels.end();) {
            auto unusedTimeInMs = (double)std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - it->second).count();
            if (unusedTimeInMs >= OLD_MODELS_TIME_MS) {
                it = unregisterModel(it);
            } else {
                ++it;
            }
        }
    }

    void ModelSetDisplayer::alterRegisterModelLastUsageTime(Model* model, std::chrono::steady_clock::time_point lastUsageTime) {
        if (!renderTarget || !renderTarget->isTestMode()) {
            throw std::runtime_error("Alter registered model last usage time not allowed outside test mode");
        }

        RegisteredModelIt itFind = registeredModels.find(model);
        if (itFind != registeredModels.end()) {
            itFind->second = lastUsageTime;
        }
    }

    void ModelSetDisplayer::addModelToDisplay(Model* modelToDisplay, std::bitset<8> layersMask) {
        bool modelRegistered = registerModel(modelToDisplay);

        if (modelRegistered && (!meshFilter || meshFilter->isAccepted(*modelToDisplay))) {
            ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*modelToDisplay);
            modelInstanceDisplayer->updateLayersMask(modelInstanceDisplayer->getLayersMask() | layersMask);

            modelsToDisplay.push_back(modelToDisplay);
        }
    }

    void ModelSetDisplayer::setModelsToDisplay(std::span<Model* const> modelsToDisplay) {
        resetModelsToDisplay();
        for (Model* modelToDisplay : modelsToDisplay) {
            addModelToDisplay(modelToDisplay);
        }
    }

    void ModelSetDisplayer::resetModelsToDisplay() {
        this->modelsToDisplay.clear();

        for (const std::unique_ptr<ModelInstanceDisplayer>& modelInstanceDisplayer : std::views::values(exclusiveInstanceDisplayers)) {
            modelInstanceDisplayer->updateLayersMask(std::bitset<8>(0));
        }
        for (const std::unique_ptr<ModelInstanceDisplayer>& modelInstanceDisplayer : std::views::values(shareableInstanceDisplayers)) {
            modelInstanceDisplayer->updateLayersMask(std::bitset<8>(0));
        }
    }

    std::span<Model* const> ModelSetDisplayer::getModelsToDisplay() const {
        return modelsToDisplay;
    }

    void ModelSetDisplayer::prepareRendering(unsigned int renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        ScopeProfiler sp(Profiler::graphic(), "modelPreRender");

        if (!isInitialized) {
            throw std::runtime_error("Model set displayer must be initialized before call display");
        }

        activeInstanceDisplayers.clear();
        for (Model* model: modelsToDisplay) {
            registeredModels.at(model) = std::chrono::steady_clock::now();

            ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*model);
            if (activeInstanceDisplayers.insert(modelInstanceDisplayer)) {
                modelInstanceDisplayer->resetRenderingModels();
            }
            modelInstanceDisplayer->registerRenderingModel(*model);
        }
        for (ModelInstanceDisplayer* activeModelDisplayer: activeInstanceDisplayers) {
            activeModelDisplayer->prepareRendering(renderingOrder, projectionViewMatrix, meshFilter.get());
        }

        purgeUnusedRegisteredModels();
    }

    void ModelSetDisplayer::prepareRendering(unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix, const ModelSortFunction& modelSorter, const void* userData) {
        ScopeProfiler sp(Profiler::graphic(), "modelPreRender");

        if (!isInitialized) {
            throw std::runtime_error("Model set displayer must be initialized before call display");
        }

        std::ranges::sort(modelsToDisplay, [&modelSorter, &userData](const Model* model1, const Model* model2) {
            return modelSorter(model1, model2, userData);
        });
        for (Model* model: modelsToDisplay) {
            registeredModels.at(model) = std::chrono::steady_clock::now();

            ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*model);
            modelInstanceDisplayer->resetRenderingModels();
            modelInstanceDisplayer->registerRenderingModel(*model);

            renderingOrder++;
            modelInstanceDisplayer->prepareRendering(renderingOrder, projectionViewMatrix, meshFilter.get());
        }
    }

    void ModelSetDisplayer::drawBBox(GeometryContainer& geometryContainer) {
        for (Model* model : modelsToDisplay) {
            registeredModels.at(model) = std::chrono::steady_clock::now();
            findModelInstanceDisplayer(*model)->drawBBox(geometryContainer);
        }
    }

    void ModelSetDisplayer::drawBaseBones(GeometryContainer& geometryContainer) {
        for (Model* model : modelsToDisplay) {
            if (model->getConstMeshes()) {
                registeredModels.at(model) = std::chrono::steady_clock::now();
                findModelInstanceDisplayer(*model)->drawBaseBones(geometryContainer, meshFilter.get());
            }
        }
    }

}
