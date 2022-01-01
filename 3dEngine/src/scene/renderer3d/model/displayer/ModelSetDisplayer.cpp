#include <algorithm>
#include <stdexcept>

#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <api/render/shader/builder/ShaderBuilder.h>

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
        clearModelInstanceDisplayers();
    }

    void ModelSetDisplayer::initialize(RenderTarget& renderTarget) {
        if (isInitialized) {
            throw std::runtime_error("Model displayer is already initialized.");
        }
        this->renderTarget = &renderTarget;

        if (renderTarget.isValidRenderTarget()) {
            if (displayMode == DisplayMode::DEFAULT_MODE) {
                if (fragmentShaderName.empty()) {
                    //use default fragment shader
                    fragmentShaderName = "model.frag.spv";
                }
                modelShader = ShaderBuilder::createShader("model.vert.spv", geometryShaderName, fragmentShaderName, std::move(shaderConstants));
            } else if (displayMode == DisplayMode::DEPTH_ONLY_MODE) {
                assert(!fragmentShaderName.empty());
                modelShader = ShaderBuilder::createShader("modelDepthOnly.vert.spv", geometryShaderName, fragmentShaderName, std::move(shaderConstants));
            } else {
                throw std::invalid_argument("Unknown display mode: " + std::to_string((int)displayMode));
            }
        } else {
            modelShader = ShaderBuilder::createNullShader();
        }

        isInitialized = true;
    }

    void ModelSetDisplayer::setupShader(const std::string& geometryShaderName, const std::string& fragmentShaderName, std::unique_ptr<ShaderConstants> shaderConstants) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set custom shader once the model displayer initialized.");
        }

        this->geometryShaderName = geometryShaderName;
        this->fragmentShaderName = fragmentShaderName;
        this->shaderConstants = std::move(shaderConstants);
    }

    void ModelSetDisplayer::setupCustomShaderVariable(std::unique_ptr<CustomModelShaderVariable> customShaderVariable) {
        this->customShaderVariable = std::move(customShaderVariable);
        clearModelInstanceDisplayers();
    }

    void ModelSetDisplayer::setupDepthOperations(bool depthTestEnabled, bool depthWriteEnabled) {
        this->depthTestEnabled = depthTestEnabled;
        this->depthWriteEnabled = depthWriteEnabled;
        clearModelInstanceDisplayers();
    }

    void ModelSetDisplayer::setupFaceCull(bool enableFaceCull) {
        this->enableFaceCull = enableFaceCull;
        clearModelInstanceDisplayers();
    }

    void ModelSetDisplayer::setupBlendFunctions(const std::vector<BlendFunction>& blendFunctions) {
        this->blendFunctions = blendFunctions;
        clearModelInstanceDisplayers();
    }

    void ModelSetDisplayer::setupMeshFilter(std::unique_ptr<MeshFilter> meshFilter) {
        this->meshFilter = std::move(meshFilter);
    }

    ModelInstanceDisplayer* ModelSetDisplayer::findModelInstanceDisplayer(const Model& model) const {
        for (ModelInstanceDisplayer* modelInstanceDisplayer : model.getModelInstanceDisplayers()) {
            if (&modelInstanceDisplayer->getModelSetDisplayer() == this) {
                return modelInstanceDisplayer;
            }
        }
        return nullptr;
    }

    void ModelSetDisplayer::clearModelInstanceDisplayers() {
        modelDisplayers.clear();
        modelInstanceDisplayers.clear();
    }

    void ModelSetDisplayer::updateModels(const std::vector<Model*>& models) {
        ScopeProfiler sp(Profiler::graphic(), "updateModels");
        assert(renderTarget);

        this->models.clear();

        for (Model* model : models) {
            if (!meshFilter || meshFilter->isAccepted(*model)) {
                this->models.push_back(model);

                ModelInstanceDisplayer* currentModelInstanceDisplayer = findModelInstanceDisplayer(*model);
                std::size_t modelInstanceId = model->computeInstanceId(displayMode); //TODO check perf

                if (currentModelInstanceDisplayer) {
                    if (currentModelInstanceDisplayer->getInstanceId() == modelInstanceId) {
                        continue; //current model displayer is still valid for the model
                    }

                    model->detachModelInstanceDisplayer(*currentModelInstanceDisplayer);
                    //TODO clean: currentModelDisplayer->getInstanceCount() == 0;
                }

                if (modelInstanceId != ModelDisplayable::INSTANCING_DENY_ID) {
                    const auto& itModelInstanceDisplayer = modelInstanceDisplayers.find(modelInstanceId);
                    if (itModelInstanceDisplayer != modelInstanceDisplayers.end()) {
                        model->attachModelInstanceDisplayer(*itModelInstanceDisplayer->second);
                        continue; //an existing model displayer has been found for the model
                    }
                }

                auto modelInstanceDisplayer = std::make_unique<ModelInstanceDisplayer>(*this, *model, displayMode, *renderTarget, *modelShader);
                modelInstanceDisplayer->setupCustomShaderVariable(customShaderVariable.get());
                modelInstanceDisplayer->setupDepthOperations(depthTestEnabled, depthWriteEnabled);
                modelInstanceDisplayer->setupBlendFunctions(blendFunctions);
                modelInstanceDisplayer->setupFaceCull(enableFaceCull);
                modelInstanceDisplayer->initialize();
                if (modelInstanceId == ModelDisplayable::INSTANCING_DENY_ID) {
                    modelDisplayers.try_emplace(model, std::move(modelInstanceDisplayer));
                } else {
                    modelInstanceDisplayers.try_emplace(modelInstanceId, std::move(modelInstanceDisplayer));
                }
            }
        }
    }

    void ModelSetDisplayer::removeModel(Model* modelToRemove) {
        if (modelToRemove) {
            modelDisplayers.erase(modelToRemove);
            ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*modelToRemove);
            if (modelInstanceDisplayer) {
                modelToRemove->detachModelInstanceDisplayer(*modelInstanceDisplayer);
                //TODO clean: modelDisplayer->getInstanceCount() == 0;
            }

            std::erase_if(models, [modelToRemove](const Model* m){return m == modelToRemove;});
        }
    }

    const std::vector<Model*>& ModelSetDisplayer::getModels() const {
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

        for (Model* model : models) {
            ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*model);
            assert(modelInstanceDisplayer);
            modelInstanceDisplayer->prepareRendering(renderingOrder, projectionViewMatrix, meshFilter.get());
        }
    }

    void ModelSetDisplayer::drawBBox(GeometryContainer& geometryContainer) const {
        for (const auto& model : models) {
            ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*model);
            assert(modelInstanceDisplayer);
            modelInstanceDisplayer->drawBBox(geometryContainer);
        }
    }

    void ModelSetDisplayer::drawBaseBones(GeometryContainer& geometryContainer) const {
        for (const auto& model : models) {
            if (model->getConstMeshes()) {
                ModelInstanceDisplayer* modelInstanceDisplayer = findModelInstanceDisplayer(*model);
                assert(modelInstanceDisplayer);
                modelInstanceDisplayer->drawBaseBones(geometryContainer, meshFilter.get());
            }
        }
    }

}
