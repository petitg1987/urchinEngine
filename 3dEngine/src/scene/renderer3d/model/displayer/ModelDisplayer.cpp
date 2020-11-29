#include <algorithm>
#include <stdexcept>

#include "ModelDisplayer.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {
    /**
     * @param displayMode Defines a display mode allowing to choose what we display and how we display models.
     * The available display modes are:
     *  - DEFAULT_MODE provide:
     *      * Depth information
     *      * Diffuse information (output unit: 0)
     *      * Normal information and ambient factor (output unit: 1)
     *  - DEPTH_ONLY_MODE provide:
     *      * Depth information
     */
    ModelDisplayer::ModelDisplayer(DisplayMode displayMode) :
        isInitialized(false),
        displayMode(displayMode),
        customShaderVariable(nullptr),
        customModelShaderVariable(nullptr),
        renderTarget(nullptr) {

    }

    void ModelDisplayer::initialize() {
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
            meshParameter.setNeedRenderTextures(true);
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
            meshParameter.setNeedRenderTextures(false);
            meshParameter.setAmbientFactorShaderVar(ShaderVar());
        } else {
            throw std::invalid_argument("Unknown display mode.");
        }

        //default matrix
        projectionMatrix = Matrix4<float>();
        ShaderDataSender().sendData(mProjectionShaderVar, projectionMatrix);

        isInitialized = true;
    }

    void ModelDisplayer::createShader(const std::string& vertexShaderName, const std::string& geometryShaderName, const std::string& fragmentShaderName) {
        std::map<std::string, std::string> shaderTokens;
        shaderTokens.insert(fragmentTokens.begin(), fragmentTokens.end());
        shaderTokens.insert(geometryTokens.begin(), geometryTokens.end());

        modelShader = ShaderBuilder().createShader(vertexShaderName, geometryShaderName, fragmentShaderName, shaderTokens);

        mProjectionShaderVar = ShaderVar(modelShader, "mProjection");
        mModelShaderVar = ShaderVar(modelShader, "mModel");
        mViewShaderVar = ShaderVar(modelShader, "mView");
    }

    void ModelDisplayer::onCameraProjectionUpdate(const Camera* camera) {
        this->projectionMatrix = camera->getProjectionMatrix();

        ShaderDataSender().sendData(mProjectionShaderVar, projectionMatrix);
    }

    ShaderVar ModelDisplayer::getShaderVar(const std::string& name) const {
        return ShaderVar(modelShader, std::string(name));
    }

    void ModelDisplayer::setCustomGeometryShader(const std::string& geometryShaderName, const std::map<std::string, std::string>& geometryTokens) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set custom geometry shader once the model displayer initialized.");
        }

        this->geometryShaderName = geometryShaderName;
        this->geometryTokens = geometryTokens;
    }

    void ModelDisplayer::setCustomFragmentShader(const std::string& fragmentShaderName, const std::map<std::string, std::string>& fragmentTokens) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set custom fragment shader once the model displayer initialized.");
        }

        this->fragmentShaderName = fragmentShaderName;
        this->fragmentTokens = fragmentTokens;
    }

    void ModelDisplayer::setCustomShaderVariable(CustomShaderVariable* customShaderVariable) {
        this->customShaderVariable = customShaderVariable;
    }

    void ModelDisplayer::setCustomModelShaderVariable(CustomModelShaderVariable* customModelShaderVariable) {
        this->customModelShaderVariable = customModelShaderVariable;
    }

    void ModelDisplayer::setRenderTarget(const RenderTarget* renderTarget) {
        this->renderTarget = renderTarget;
    }

    void ModelDisplayer::setModels(const std::vector<Model*>& models) {
        this->models = models;
    }

    void ModelDisplayer::updateAnimation(float dt) {
        ScopeProfiler profiler("3d", "updateAnimation");

        for (auto model : models) {
            model->updateAnimation(dt);
        }
    }

    void ModelDisplayer::display(const Matrix4<float>& viewMatrix) {
        ScopeProfiler profiler("3d", "modelDisplay");

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

            model->display(renderTarget, meshParameter);
        }
    }

    void ModelDisplayer::drawBBox(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) const {
        for (auto model : models) {
            model->drawBBox(renderTarget, projectionMatrix, viewMatrix);
        }
    }

    void ModelDisplayer::drawBaseBones(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix, const std::string& meshFilename) const {
        for (auto model : models) {
            if (model->getMeshes() && model->getMeshes()->getMeshFilename()==meshFilename) {
                model->drawBaseBones(renderTarget, projectionMatrix, viewMatrix);
            }
        }
    }

}
