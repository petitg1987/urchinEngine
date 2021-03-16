#include <algorithm>
#include <stdexcept>

#include "ModelSetDisplayer.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    ModelSetDisplayer::ModelSetDisplayer(DisplayMode displayMode) :
            isInitialized(false),
            displayMode(displayMode),
            customShaderVariable(nullptr),
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

            mNormalShaderVar = ShaderVar(modelShader, "mNormal");
            ambientFactorShaderVar = ShaderVar(modelShader, "ambientFactor");

            int diffuseTexUnit = 0;
            int normalTexUnit = 1;
            ShaderDataSender()
                .sendData(ShaderVar(modelShader, "diffuseTex"), diffuseTexUnit)
                .sendData(ShaderVar(modelShader, "normalTex"), normalTexUnit);

            //setup mesh parameters
            meshParameter.setAmbientFactorShaderVar(ambientFactorShaderVar);
        } else if (displayMode == DEPTH_ONLY_MODE) {
            //shader creation
            std::string vertexShaderName = "modelDepthOnly.vert";
            if (fragmentShaderName.empty()) { //use default fragment shader
                fragmentShaderName = "modelDepthOnly.frag";
            }
            createShader(vertexShaderName, geometryShaderName, fragmentShaderName);

            mNormalShaderVar = ShaderVar();
            ambientFactorShaderVar = ShaderVar();

            //setup mesh parameters
            meshParameter.setAmbientFactorShaderVar(ShaderVar());
        } else {
            throw std::invalid_argument("Unknown display mode.");
        }

        //default matrix
        projectionMatrix = Matrix4<float>();
        ShaderDataSender().sendData(mProjectionShaderVar, projectionMatrix);

        this->renderTarget = std::move(renderTarget);

        isInitialized = true;
    }

    void ModelSetDisplayer::createShader(const std::string& vertexShaderName, const std::string& geometryShaderName, const std::string& fragmentShaderName) {
        std::map<std::string, std::string> shaderTokens;
        shaderTokens.insert(fragmentTokens.begin(), fragmentTokens.end());
        shaderTokens.insert(geometryTokens.begin(), geometryTokens.end());

        modelShader = ShaderBuilder().createShader(vertexShaderName, geometryShaderName, fragmentShaderName, shaderTokens);

        mProjectionShaderVar = ShaderVar(modelShader, "mProjection");
        mModelShaderVar = ShaderVar(modelShader, "mModel");
        mViewShaderVar = ShaderVar(modelShader, "mView");
    }

    void ModelSetDisplayer::onCameraProjectionUpdate(const Camera* camera) {
        this->projectionMatrix = camera->getProjectionMatrix();

        ShaderDataSender().sendData(mProjectionShaderVar, projectionMatrix);
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

    void ModelSetDisplayer::setCustomShaderVariable(CustomShaderVariable* customShaderVariable) {
        this->customShaderVariable = customShaderVariable;
    }

    void ModelSetDisplayer::setCustomModelShaderVariable(CustomModelShaderVariable* customModelShaderVariable) {
        this->customModelShaderVariable = customModelShaderVariable;
    }

    void ModelSetDisplayer::setModels(const std::vector<Model*>& models) {
        for (auto model : models) {
            const auto& itModel = modelsDisplayer.find(model);
            if (itModel == modelsDisplayer.end()) {
                modelsDisplayer.emplace(std::make_pair(model, std::make_unique<ModelDisplayer>(model, displayMode, renderTarget, modelShader)));
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
        }
        if (!renderTarget) {
            throw std::runtime_error("Render target must be specified before call display");
        }

        ShaderDataSender().sendData(mViewShaderVar, viewMatrix);
        if (customShaderVariable) {
            customShaderVariable->loadCustomShaderVariables();
        }

        renderTarget->activeShader(modelShader);
        for (const auto& model : models) {
            ShaderDataSender().sendData(mModelShaderVar, model->getTransform().getTransformMatrix());
            if (displayMode == DEFAULT_MODE) {
                ShaderDataSender().sendData(mNormalShaderVar, model->getTransform().getTransformMatrix().toMatrix3().inverse().transpose());
            }
            if (customModelShaderVariable) {
                customModelShaderVariable->loadCustomShaderVariables(model);
            }

            modelsDisplayer.at(model)->display(meshParameter);
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
