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
        //TODO clean all displayer from Model
        modelsDisplayer.clear();
        instancingModelsDisplayer.clear();
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
        modelsDisplayer.clear(); //TODO create method: clearModelDisplayer
        instancingModelsDisplayer.clear();
    }

    void ModelSetDisplayer::setupDepthOperations(bool depthTestEnabled, bool depthWriteEnabled) {
        this->depthTestEnabled = depthTestEnabled;
        this->depthWriteEnabled = depthWriteEnabled;
        modelsDisplayer.clear();
        instancingModelsDisplayer.clear();
    }

    void ModelSetDisplayer::setupFaceCull(bool enableFaceCull) {
        this->enableFaceCull = enableFaceCull;
        modelsDisplayer.clear();
        instancingModelsDisplayer.clear();
    }

    void ModelSetDisplayer::setupBlendFunctions(const std::vector<BlendFunction>& blendFunctions) {
        this->blendFunctions = blendFunctions;
        modelsDisplayer.clear();
        instancingModelsDisplayer.clear();
    }

    void ModelSetDisplayer::setupMeshFilter(std::unique_ptr<MeshFilter> meshFilter) {
        this->meshFilter = std::move(meshFilter);
    }

    ModelDisplayer* ModelSetDisplayer::findModelDisplayer(const Model& model) const {
        for (ModelDisplayer* modelDisplayer : model.getModelDisplayers()) {
            if (&modelDisplayer->getModelSetDisplayer() == this) {
                return modelDisplayer;
            }
        }
        return nullptr;
    }

    void ModelSetDisplayer::updateModels(const std::vector<Model*>& models) {
        ScopeProfiler sp(Profiler::graphic(), "updateModels");
        assert(renderTarget);

        this->models.clear();

        for (Model* model : models) {
            if (!meshFilter || meshFilter->isAccepted(*model)) {
                this->models.push_back(model);

                ModelDisplayer* currentModelDisplayer = findModelDisplayer(*model);
                std::size_t modelInstanceId = model->computeInstanceId(displayMode);

                if (currentModelDisplayer) {
                    if (currentModelDisplayer->getInstanceId() == modelInstanceId) {
                        continue; //current model displayer is still valid for the model
                    }

                    model->detachModelDisplayer(*currentModelDisplayer);
                    //TODO clean: currentModelDisplayer->getInstanceCount() == 0;
                }

                if (modelInstanceId != ModelDisplayable::INSTANCING_DENY_ID) {
                    const auto& itModelDisplayer = instancingModelsDisplayer.find(modelInstanceId);
                    if (itModelDisplayer != instancingModelsDisplayer.end()) {
                        model->attachModelDisplayer(*itModelDisplayer->second);
                        continue; //an existing model displayer has been found for the model
                    }
                }

                auto modelDisplayer = std::make_unique<ModelDisplayer>(*this, *model, displayMode, *renderTarget, *modelShader);
                modelDisplayer->setupCustomShaderVariable(customShaderVariable.get());
                modelDisplayer->setupDepthOperations(depthTestEnabled, depthWriteEnabled);
                modelDisplayer->setupBlendFunctions(blendFunctions);
                modelDisplayer->setupFaceCull(enableFaceCull);
                modelDisplayer->initialize();
                if (modelInstanceId == ModelDisplayable::INSTANCING_DENY_ID) {
                    modelsDisplayer.try_emplace(model, std::move(modelDisplayer));
                } else {
                    instancingModelsDisplayer.try_emplace(modelInstanceId, std::move(modelDisplayer));
                }
            }
        }
    }

    void ModelSetDisplayer::removeModel(Model* model) {
        if (model) {
            ModelDisplayer* modelDisplayer = findModelDisplayer(*model);
            if (modelDisplayer) {
                model->detachModelDisplayer(*modelDisplayer);
            }

            //TODO remove model from models ?
            assert(false);
        }
    }

    const std::vector<Model*>& ModelSetDisplayer::getModels() const {
        return models;
    }

    bool ModelSetDisplayer::isModelDisplayerExist(const Model& model) const {
        return findModelDisplayer(model) != nullptr;
    }

    void ModelSetDisplayer::prepareRendering(unsigned int renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        ScopeProfiler sp(Profiler::graphic(), "modelPreRender");

        if (!isInitialized) {
            throw std::runtime_error("Model displayer must be initialized before call display");
        } else if (!renderTarget) {
            throw std::runtime_error("Render target must be specified before call display");
        }

        for (Model* model : models) {
            ModelDisplayer* modelDisplayer = findModelDisplayer(*model);
            assert(modelDisplayer);
            modelDisplayer->prepareRendering(renderingOrder, projectionViewMatrix, meshFilter.get());
        }
    }

    void ModelSetDisplayer::drawBBox(GeometryContainer& geometryContainer) const {
        for (const auto& model : models) {
            ModelDisplayer* modelDisplayer = findModelDisplayer(*model);
            assert(modelDisplayer);
            modelDisplayer->drawBBox(geometryContainer);
        }
    }

    void ModelSetDisplayer::drawBaseBones(GeometryContainer& geometryContainer) const {
        for (const auto& model : models) {
            if (model->getConstMeshes()) {
                ModelDisplayer* modelDisplayer = findModelDisplayer(*model);
                assert(modelDisplayer);
                modelDisplayer->drawBaseBones(geometryContainer, meshFilter.get());
            }
        }
    }

}
