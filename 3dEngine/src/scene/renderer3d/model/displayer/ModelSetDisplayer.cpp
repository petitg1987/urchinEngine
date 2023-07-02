#include <algorithm>
#include <stdexcept>

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

        if (renderTarget.isValidRenderTarget()) {
            assert(!vertexShaderName.empty());
            assert(!fragmentShaderName.empty());
            modelShader = ShaderBuilder::createShader(vertexShaderName, geometryShaderName, fragmentShaderName, std::move(shaderConstants));
        } else {
            modelShader = ShaderBuilder::createNullShader();
        }

        isInitialized = true;
    }

    void ModelSetDisplayer::setupShader(std::string vertexShaderName, std::string geometryShaderName, std::string fragmentShaderName, std::unique_ptr<ShaderConstants> shaderConstants) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set custom shader once the model displayer initialized.");
        }

        this->vertexShaderName = std::move(vertexShaderName);
        this->geometryShaderName = std::move(geometryShaderName);
        this->fragmentShaderName = std::move(fragmentShaderName);
        this->shaderConstants = std::move(shaderConstants);
    }

    void ModelSetDisplayer::setupCustomShaderVariable(std::unique_ptr<CustomModelShaderVariable> customShaderVariable) {
        this->customShaderVariable = std::move(customShaderVariable);
        clearDisplayers();
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

    void ModelSetDisplayer::setupMeshFilter(std::unique_ptr<MeshFilter> meshFilter) {
        this->meshFilter = std::move(meshFilter);
    }

    void ModelSetDisplayer::notify(Observable* observable, int notificationType) {
        if (const Model* model = dynamic_cast<Model*>(observable)) {
            ModelInstanceDisplayer* displayer = findModelInstanceDisplayer(*model);
            if (!displayer) {
                return;
            }

            bool canUpdateDisplayer = false;
            std::size_t newModelInstanceId = model->computeInstanceId(displayMode);
            if (newModelInstanceId != ModelDisplayable::INSTANCING_DENY_ID && displayer->getInstanceId() != ModelDisplayable::INSTANCING_DENY_ID) {
                if (displayer->getInstanceCount() <= 1 && !modelInstanceDisplayers.contains(newModelInstanceId)) {
                    //case: displayer is not shared and there isn't other displayer matching the new instance ID
                    canUpdateDisplayer = true;

                    auto displayerNodeHandler = modelInstanceDisplayers.extract(displayer->getInstanceId());
                    assert(displayerNodeHandler.mapped().get() == displayer);
                    displayerNodeHandler.mapped()->updateInstanceId(newModelInstanceId);
                    displayerNodeHandler.key() = newModelInstanceId;
                    modelInstanceDisplayers.insert(std::move(displayerNodeHandler));
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
        for (const auto& [model, displayer] : modelDisplayers) {
            unobserveModelUpdate(*model);
        }
        modelDisplayers.clear();

        for (const auto& [instanceId, displayer] : modelInstanceDisplayers) {
            for (Model* model : displayer->getInstanceModels()) {
                unobserveModelUpdate(*model);
            }
        }
        modelInstanceDisplayers.clear();
    }

    void ModelSetDisplayer::removeModelFromDisplayer(Model& model, ModelInstanceDisplayer& modelInstanceDisplayer) {
        modelInstanceDisplayer.removeInstanceModel(model);
        unobserveModelUpdate(model);

        if (modelInstanceDisplayer.getInstanceCount() == 0) {
            //to do:
            // - remove the displayer based on strategy to define (unused for x seconds, too much unused displayers, etc.)
            // - erase displayer from modelDisplayers/modelInstanceDisplayers
        }
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

    void ModelSetDisplayer::updateModels(std::span<Model* const> models) {
        ScopeProfiler sp(Profiler::graphic(), "updateModels");
        assert(renderTarget);

        this->models.clear();

        for (Model* model : models) {
            if (!meshFilter || meshFilter->isAccepted(*model)) {
                this->models.push_back(model);
                std::size_t modelInstanceId = model->computeInstanceId(displayMode);

                ModelInstanceDisplayer* currentModelInstanceDisplayer = findModelInstanceDisplayer(*model);
                if (currentModelInstanceDisplayer) {
                    if (currentModelInstanceDisplayer->getInstanceId() == modelInstanceId) {
                        continue; //the model displayer attached to the model is still valid
                    }
                    removeModelFromDisplayer(*model, *currentModelInstanceDisplayer);
                }

                if (modelInstanceId == ModelDisplayable::INSTANCING_DENY_ID) {
                    const auto& itFind = modelDisplayers.find(model);
                    if (itFind != modelDisplayers.end()) {
                        assert(itFind->second->getInstanceCount() == 0);
                        addModelToDisplayer(*model, *itFind->second);
                        continue; //the model displayer used in past for this model has been found
                    }
                } else {
                    const auto& itFind = modelInstanceDisplayers.find(modelInstanceId);
                    if (itFind != modelInstanceDisplayers.end()) {
                        addModelToDisplayer(*model, *itFind->second);
                        continue; //a matching model instance displayer has been found for the model
                    }
                }

                auto modelInstanceDisplayer = std::make_unique<ModelInstanceDisplayer>(*this, displayMode, *renderTarget, *modelShader);
                modelInstanceDisplayer->setupCustomShaderVariable(customShaderVariable.get());
                modelInstanceDisplayer->setupDepthOperations(depthTestEnabled, depthWriteEnabled);
                modelInstanceDisplayer->setupBlendFunctions(blendFunctions);
                modelInstanceDisplayer->setupFaceCull(enableFaceCull);
                addModelToDisplayer(*model, *modelInstanceDisplayer);
                modelInstanceDisplayer->initialize();
                if (modelInstanceId == ModelDisplayable::INSTANCING_DENY_ID) {
                    modelDisplayers.try_emplace(model, std::move(modelInstanceDisplayer));
                } else {
                    modelInstanceDisplayers.try_emplace(modelInstanceId, std::move(modelInstanceDisplayer));
                }
            }
        }
    }

    void ModelSetDisplayer::removeModel(Model* model) {
        if (model) {
            modelDisplayers.erase(model);
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

        activeModelDisplayers.clear();
        for (const Model* model: models) {
            ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*model);
            if (activeModelDisplayers.insert(modelInstanceDisplayer).second) {
                modelInstanceDisplayer->resetRenderingModels();
            }
            modelInstanceDisplayer->registerRenderingModel(*model);
        }
        for (const ModelInstanceDisplayer* activeModelDisplayer: activeModelDisplayers) {
            activeModelDisplayer->prepareRendering(renderingOrder, projectionViewMatrix, meshFilter.get());
        }
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
