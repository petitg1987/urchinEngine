#include <algorithm>
#include <stdexcept>

#include "ModelSetDisplayer.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    ModelSetDisplayer::ModelSetDisplayer(DisplayMode displayMode) :
            isInitialized(false),
            displayMode(displayMode),
            customModelShaderVariable(nullptr),
            renderTarget(nullptr) {

    }

    ModelSetDisplayer::~ModelSetDisplayer() {
        modelsDisplayer.clear();
    }

    void ModelSetDisplayer::initialize(std::shared_ptr<RenderTarget> renderTarget) {
        if (isInitialized) {
            throw std::runtime_error("Model displayer is already initialized.");
        }

        if (displayMode == DEFAULT_MODE) {
            //shader creation
            std::string vertexShaderName = "model.vert";
            if (fragmentShaderName.empty()) { //use default fragment shader
                fragmentShaderName = "model.frag";
            }
            createShader(vertexShaderName, geometryShaderName, fragmentShaderName);

            int diffuseTexUnit = 0;
            int normalTexUnit = 1;
            ShaderDataSender(true)
                .sendData(ShaderVar(modelShader, "diffuseTex"), diffuseTexUnit) //binding 20
                .sendData(ShaderVar(modelShader, "normalTex"), normalTexUnit); //binding 21
        } else if (displayMode == DEPTH_ONLY_MODE) {
            //shader creation
            std::string vertexShaderName = "modelDepthOnly.vert";
            if (fragmentShaderName.empty()) { //use default fragment shader
                fragmentShaderName = "modelDepthOnly.frag";
            }
            createShader(vertexShaderName, geometryShaderName, fragmentShaderName);
        } else {
            throw std::invalid_argument("Unknown display mode: " + std::to_string(displayMode));
        }

        this->renderTarget = std::move(renderTarget);

        isInitialized = true;
    }

    void ModelSetDisplayer::createShader(const std::string& vertexShaderName, const std::string& geometryShaderName, const std::string& fragmentShaderName) {
        std::map<std::string, std::string> shaderTokens;
        shaderTokens.insert(fragmentTokens.begin(), fragmentTokens.end());
        shaderTokens.insert(geometryTokens.begin(), geometryTokens.end());

        modelShader = ShaderBuilder().createShader(vertexShaderName, geometryShaderName, fragmentShaderName, shaderTokens);
    }

    void ModelSetDisplayer::onCameraProjectionUpdate(const Camera* camera) {
        this->projectionMatrix = camera->getProjectionMatrix();

        for (auto& modelDisplayer : modelsDisplayer) {
            modelDisplayer.second->onCameraProjectionUpdate(camera);
        }
    }

    ShaderVar ModelSetDisplayer::getShaderVar(const std::string& name) const {
        return ShaderVar(modelShader, std::string(name));
    }

    void ModelSetDisplayer::setCustomGeometryShader(const std::string& geometryShaderName, const std::map<std::string, std::string>& geometryTokens) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set custom geometry shader once the model displayer initialized.");
        }

        this->geometryShaderName = geometryShaderName;
        this->geometryTokens = geometryTokens;
    }

    void ModelSetDisplayer::setCustomFragmentShader(const std::string& fragmentShaderName, const std::map<std::string, std::string>& fragmentTokens) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set custom fragment shader once the model displayer initialized.");
        }

        this->fragmentShaderName = fragmentShaderName;
        this->fragmentTokens = fragmentTokens;
    }

    void ModelSetDisplayer::setCustomModelShaderVariable(CustomModelShaderVariable* customModelShaderVariable) {
        this->customModelShaderVariable = customModelShaderVariable;

        modelsDisplayer.clear();
    }

    void ModelSetDisplayer::setModels(const std::vector<Model*>& models) {
        for (auto model : models) {
            const auto& itModel = modelsDisplayer.find(model);
            if (itModel == modelsDisplayer.end()) {
                auto modelDisplayer = std::make_unique<ModelDisplayer>(model, projectionMatrix, displayMode, renderTarget, modelShader, customModelShaderVariable);
                modelsDisplayer.emplace(std::make_pair(model, std::move(modelDisplayer)));
            }
        }

        this->models = models;
    }

    void ModelSetDisplayer::removeModel(Model* model) {
        modelsDisplayer.erase(model);
    }

    void ModelSetDisplayer::updateAnimation(float dt) {
        ScopeProfiler sp(Profiler::graphic(), "updateAnimation");

        for (auto model : models) {
            model->updateAnimation(dt);
        }
    }

    void ModelSetDisplayer::display(const Matrix4<float>& viewMatrix) {
        ScopeProfiler sp(Profiler::graphic(), "modelDisplay");

        if (!isInitialized) {
            throw std::runtime_error("Model displayer must be initialized before call display");
        } else if (!renderTarget) {
            throw std::runtime_error("Render target must be specified before call display");
        }

        for (auto model : models) {
            modelsDisplayer.at(model)->display(viewMatrix);
        }
    }

    void ModelSetDisplayer::drawBBox(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) const {
        for (const auto& model : models) {
            modelsDisplayer.at(model)->drawBBox(projectionMatrix, viewMatrix);
        }
    }

    void ModelSetDisplayer::drawBaseBones(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix, const std::string& meshFilename) const {
        for (const auto& model : models) {
            if (model->getConstMeshes() && model->getConstMeshes()->getMeshFilename() == meshFilename) {
                modelsDisplayer.at(model)->drawBaseBones(projectionMatrix, viewMatrix);
            }
        }
    }

}
