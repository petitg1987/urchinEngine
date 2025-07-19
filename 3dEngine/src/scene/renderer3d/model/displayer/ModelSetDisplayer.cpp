#include <algorithm>
#include <stdexcept>
#include <ranges>

#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <graphics/render/shader/ShaderBuilder.h>

namespace urchin {

    ModelSetDisplayer::ModelSetDisplayer(DisplayMode displayMode) :
            isInitialized(false),
            displayMode(displayMode),
            customShaderVariable(nullptr),
            depthTestEnabled(true),
            depthWriteEnabled(true),
            enableFaceCull(true),
            enableLayerIndexDataInShader(false),
            textureReaders({}),
            renderTarget(nullptr) {

    }

    ModelSetDisplayer::~ModelSetDisplayer() {
        clearDisplayers();
    }

    void ModelSetDisplayer::initialize(RenderTarget& renderTarget) {
        if (isInitialized) {
            throw std::runtime_error("Model displayer is already initialized.");
        }
        this->renderTarget = &renderTarget;

        assert(!vertexShaderName.empty());
        assert(!fragmentShaderName.empty());
        modelShader = ShaderBuilder::createShader(vertexShaderName, fragmentShaderName, std::move(shaderConstants), renderTarget.isTestMode());

        isInitialized = true;
    }

    void ModelSetDisplayer::setupShader(std::string vertexShaderName, std::string fragmentShaderName, std::unique_ptr<ShaderConstants> shaderConstants) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set custom shader once the model displayer initialized.");
        }

        this->vertexShaderName = std::move(vertexShaderName);
        this->fragmentShaderName = std::move(fragmentShaderName);
        this->shaderConstants = std::move(shaderConstants);
    }

    void ModelSetDisplayer::setupCustomShaderVariable(std::unique_ptr<CustomModelShaderVariable> customShaderVariable) {
        this->customShaderVariable = std::move(customShaderVariable);
        clearDisplayers();
    }

    CustomModelShaderVariable* ModelSetDisplayer::getCustomShaderVariable() const {
        return this->customShaderVariable.get();
    }

    void ModelSetDisplayer::setupDepthOperations(bool depthTestEnabled, bool depthWriteEnabled) {
        this->depthTestEnabled = depthTestEnabled;
        this->depthWriteEnabled = depthWriteEnabled;
        clearDisplayers();
    }

    void ModelSetDisplayer::setupFaceCull(bool enableFaceCull) {
        this->enableFaceCull = enableFaceCull;
        clearDisplayers();
    }

    void ModelSetDisplayer::setupBlendFunctions(const std::vector<BlendFunction>& blendFunctions) {
        this->blendFunctions = blendFunctions;
        clearDisplayers();
    }

    void ModelSetDisplayer::setupLayerIndexDataInShader(bool enableLayerIndexDataInShader) {
        this->enableLayerIndexDataInShader = enableLayerIndexDataInShader;
        clearDisplayers();
    }

    void ModelSetDisplayer::setupCustomTextures(const std::array<std::shared_ptr<TextureReader>, 2>& textureReaders) {
        this->textureReaders = textureReaders;
        clearDisplayers();
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
                }
            } else {
                //The displayer will be replaced in the 'updateModels' method, but it is already detached here.
                //Indeed, the updated value in the model could be wrongly used by another update of model in the displayer via ModelInstanceDisplayer#getReferenceModel().
                removeModelFromDisplayer(*model, *displayer);
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

    void ModelSetDisplayer::clearDisplayers() {
        for (Model* model : std::views::keys(exclusiveInstanceDisplayers)) {
            unobserveModelUpdate(*model);
        }
        exclusiveInstanceDisplayers.clear();

        for (const std::unique_ptr<ModelInstanceDisplayer>& displayer : std::views::values(shareableInstanceDisplayers)) {
            for (Model* model : displayer->getInstanceModels()) {
                unobserveModelUpdate(*model);
            }
        }
        shareableInstanceDisplayers.clear();
    }

    void ModelSetDisplayer::removeModelFromDisplayer(Model& model, ModelInstanceDisplayer& modelInstanceDisplayer) {
        modelInstanceDisplayer.removeInstanceModel(model);
        unobserveModelUpdate(model);
    }

    void ModelSetDisplayer::addModelToDisplayer(Model& model, ModelInstanceDisplayer& modelInstanceDisplayer) {
        modelInstanceDisplayer.addInstanceModel(model);
        observeModelUpdate(model);
    }

    void ModelSetDisplayer::observeModelUpdate(Model& model) {
        model.addObserver(this, Model::MESH_VERTICES_UPDATED);
        model.addObserver(this, Model::MESH_UV_UPDATED);
        model.addObserver(this, Model::MATERIAL_UPDATED);
        model.addObserver(this, Model::SCALE_UPDATED);
    }

    void ModelSetDisplayer::unobserveModelUpdate(Model& model) {
        model.removeObserver(this, Model::SCALE_UPDATED);
        model.removeObserver(this, Model::MATERIAL_UPDATED);
        model.removeObserver(this, Model::MESH_UV_UPDATED);
        model.removeObserver(this, Model::MESH_VERTICES_UPDATED);
    }

    void ModelSetDisplayer::cleanAllModels() {
        this->models.clear();

        for (const std::unique_ptr<ModelInstanceDisplayer>& modelInstanceDisplayer : std::views::values(exclusiveInstanceDisplayers)) {
            modelInstanceDisplayer->updateLayersMask(std::bitset<8>());
        }
        for (const std::unique_ptr<ModelInstanceDisplayer>& modelInstanceDisplayer : std::views::values(shareableInstanceDisplayers)) {
            modelInstanceDisplayer->updateLayersMask(std::bitset<8>());
        }
    }

    void ModelSetDisplayer::addNewModel(Model* model, std::bitset<8> layersMask) {
        assert(renderTarget);

        if (meshFilter && !meshFilter->isAccepted(*model)) {
            return;
        } else if (!model->getConstMeshes()) {
            return;
        }

        this->models.push_back(model);
        std::size_t modelInstanceId = model->computeInstanceId(displayMode);

        ModelInstanceDisplayer* currentModelInstanceDisplayer = findModelInstanceDisplayer(*model);
        if (currentModelInstanceDisplayer) {
            if (currentModelInstanceDisplayer->getInstanceId() == modelInstanceId) {
                currentModelInstanceDisplayer->updateLayersMask(currentModelInstanceDisplayer->getLayersMask() | layersMask);
                return; //the model displayer attached to the model is still valid
            }
            removeModelFromDisplayer(*model, *currentModelInstanceDisplayer);
        }

        if (modelInstanceId == ModelDisplayable::INSTANCING_DENY_ID) {
            const auto& itFind = exclusiveInstanceDisplayers.find(model);
            if (itFind != exclusiveInstanceDisplayers.end()) {
                assert(itFind->second->getInstanceCount() == 0);
                assert(itFind->second->getLayersMask().none());
                addModelToDisplayer(*model, *itFind->second);
                itFind->second->updateLayersMask(layersMask);
                return; //the model displayer used in past for this model has been found
            }
        } else {
            const auto& itFind = shareableInstanceDisplayers.find(modelInstanceId);
            if (itFind != shareableInstanceDisplayers.end()) {
                addModelToDisplayer(*model, *itFind->second);
                itFind->second->updateLayersMask(itFind->second->getLayersMask() | layersMask);
                return; //a matching model instance displayer has been found for the model
            }
        }

        auto modelInstanceDisplayer = std::make_unique<ModelInstanceDisplayer>(*this, displayMode, *renderTarget, *modelShader);
        modelInstanceDisplayer->setupCustomShaderVariable(customShaderVariable.get());
        modelInstanceDisplayer->setupDepthOperations(depthTestEnabled, depthWriteEnabled);
        modelInstanceDisplayer->setupBlendFunctions(blendFunctions);
        modelInstanceDisplayer->setupFaceCull(enableFaceCull);
        modelInstanceDisplayer->setupLayerIndexDataInShader(enableLayerIndexDataInShader);
        modelInstanceDisplayer->setupLayersMask(layersMask);
        modelInstanceDisplayer->setupCustomTextures(textureReaders);
        addModelToDisplayer(*model, *modelInstanceDisplayer);
        modelInstanceDisplayer->initialize();
        if (modelInstanceId == ModelDisplayable::INSTANCING_DENY_ID) {
            exclusiveInstanceDisplayers.try_emplace(model, std::move(modelInstanceDisplayer));
        } else {
            shareableInstanceDisplayers.try_emplace(modelInstanceId, std::move(modelInstanceDisplayer));
        }
    }

    void ModelSetDisplayer::replaceAllModels(std::span<Model* const> models) {
        ScopeProfiler sp(Profiler::graphic(), "repAllModels");

        cleanAllModels();
        for (Model* model : models) {
            addNewModel(model);
        }
    }

    void ModelSetDisplayer::removeModel(Model* model) {
        if (model) {
            exclusiveInstanceDisplayers.erase(model);
            ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*model);
            if (modelInstanceDisplayer) {
                removeModelFromDisplayer(*model, *modelInstanceDisplayer);
            }

            std::erase(models, model);
        }
    }

    std::span<Model* const> ModelSetDisplayer::getModels() const {
        return models;
    }

    bool ModelSetDisplayer::isDisplayerExist(const Model& model) const {
        return findModelInstanceDisplayer(model) != nullptr;
    }

    void ModelSetDisplayer::prepareRendering(unsigned int renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        ScopeProfiler sp(Profiler::graphic(), "modelPreRender");

        if (!isInitialized) {
            throw std::runtime_error("Model displayer must be initialized before call display");
        } else if (!renderTarget) {
            throw std::runtime_error("Render target must be specified before call display");
        }

        activeInstanceDisplayers.clear();
        for (const Model* model: models) {
            ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*model);
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
            throw std::runtime_error("Model displayer must be initialized before call display");
        } else if (!renderTarget) {
            throw std::runtime_error("Render target must be specified before call display");
        }

        std::ranges::sort(models, [&modelSorter, &userData](const Model* model1, const Model* model2) {
            return modelSorter(model1, model2, userData);
        });
        for (const Model* model: models) {
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
        for (const auto& model : models) {
            findModelInstanceDisplayer(*model)->drawBBox(geometryContainer);
        }
    }

    void ModelSetDisplayer::drawBaseBones(GeometryContainer& geometryContainer) const {
        for (const auto& model : models) {
            if (model->getConstMeshes()) {
                findModelInstanceDisplayer(*model)->drawBaseBones(geometryContainer, meshFilter.get());
            }
        }
    }

}
