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
        modelsDisplayer.clear();
    }

    void ModelSetDisplayer::initialize(RenderTarget& renderTarget) {
        if (isInitialized) {
            throw std::runtime_error("Model displayer is already initialized.");
        }
        this->renderTarget = &renderTarget;

        if (renderTarget.isValidRenderTarget()) {
            if (displayMode == DEFAULT_MODE) {
                if (fragmentShaderName.empty()) {
                    //use default fragment shader
                    fragmentShaderName = "model.frag.spv";
                }
                modelShader = ShaderBuilder::createShader("model.vert.spv", geometryShaderName, fragmentShaderName, std::move(shaderConstants));
            } else if (displayMode == DEPTH_ONLY_MODE) {
                assert(!fragmentShaderName.empty());
                modelShader = ShaderBuilder::createShader("modelDepthOnly.vert.spv", geometryShaderName, fragmentShaderName, std::move(shaderConstants));
            } else {
                throw std::invalid_argument("Unknown display mode: " + std::to_string(displayMode));
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
        modelsDisplayer.clear();
    }

    void ModelSetDisplayer::setupDepthOperations(bool depthTestEnabled, bool depthWriteEnabled) {
        this->depthTestEnabled = depthTestEnabled;
        this->depthWriteEnabled = depthWriteEnabled;
        modelsDisplayer.clear();
    }

    void ModelSetDisplayer::setupFaceCull(bool enableFaceCull) {
        this->enableFaceCull = enableFaceCull;
        modelsDisplayer.clear();
    }

    void ModelSetDisplayer::setupBlendFunctions(const std::vector<BlendFunction>& blendFunctions) {
        this->blendFunctions = blendFunctions;
        modelsDisplayer.clear();
    }

    void ModelSetDisplayer::setupMeshFilter(std::unique_ptr<MeshFilter> meshFilter) {
        this->meshFilter = std::move(meshFilter);
    }

    void ModelSetDisplayer::updateModels(const std::vector<Model*>& models) {
        ScopeProfiler sp(Profiler::graphic(), "updateModels");
        assert(renderTarget);

        this->models.clear();

        for (auto model : models) {
            if (!meshFilter || meshFilter->isAccepted(*model)) {
                const auto& itModel = modelsDisplayer.find(model);
                if (itModel == modelsDisplayer.end()) {
                    auto modelDisplayer = std::make_unique<ModelDisplayer>(model, displayMode, *renderTarget, *modelShader);
                    modelDisplayer->setupCustomShaderVariable(customShaderVariable.get());
                    modelDisplayer->setupDepthOperations(depthTestEnabled, depthWriteEnabled);
                    modelDisplayer->setupBlendFunctions(blendFunctions);
                    modelDisplayer->setupFaceCull(enableFaceCull);
                    modelDisplayer->initialize();
                    modelsDisplayer.emplace(std::make_pair(model, std::move(modelDisplayer)));
                }

                this->models.push_back(model);
            }
        }
    }

    void ModelSetDisplayer::removeModel(Model* model) {
        modelsDisplayer.erase(model);
    }

    const std::vector<Model*>& ModelSetDisplayer::getModels() const {
        return models;
    }

    bool ModelSetDisplayer::isModelDisplayerExist(const Model& model) const {
        return modelsDisplayer.find(&model) != modelsDisplayer.end();
    }

    void ModelSetDisplayer::prepareRendering(unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        ScopeProfiler sp(Profiler::graphic(), "modelPreRender");

        if (!isInitialized) {
            throw std::runtime_error("Model displayer must be initialized before call display");
        } else if (!renderTarget) {
            throw std::runtime_error("Render target must be specified before call display");
        }

        for (auto model : models) {
            modelsDisplayer.at(model)->prepareRendering(renderingOrder, projectionViewMatrix, meshFilter.get());
        }
    }

    void ModelSetDisplayer::drawBBox(GeometryContainer& geometryContainer) const {
        for (const auto& model : models) {
            modelsDisplayer.at(model)->drawBBox(geometryContainer);
        }
    }

    void ModelSetDisplayer::drawBaseBones(GeometryContainer& geometryContainer) const {
        for (const auto& model : models) {
            if (model->getConstMeshes()) {
                modelsDisplayer.at(model)->drawBaseBones(geometryContainer, meshFilter.get());
            }
        }
    }

}
