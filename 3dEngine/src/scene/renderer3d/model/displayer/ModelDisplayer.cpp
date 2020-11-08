#include <GL/glew.h>
#include <algorithm>
#include <stdexcept>

#include "ModelDisplayer.h"
#include "graphic/shader/builder/ShaderBuilder.h"

namespace urchin {
    /**
     * @param displayMode Defines a display mode allowing to choose what we display and how we display models.
     * The available display modes are:
     *  - DEFAULT_MODE provide:
     *      * Depth information
     *      * Diffuse information (output number: 0)
     *      * Normal information and ambient factor (output number: 1)
     *  - DEPTH_ONLY_MODE provide:
     *      * Depth information
     */
    ModelDisplayer::ModelDisplayer(DisplayMode displayMode) :
        isInitialized(false),
        displayMode(displayMode),
        mProjectionLoc(0),
        mModelLoc(0),
        mViewLoc(0),
        mNormalLoc(0),
        ambientFactorLoc(0),
        customUniform(nullptr),
        customModelUniform(nullptr) {

    }

    void ModelDisplayer::initialize() {
        if (isInitialized) {
            throw std::runtime_error("Model displayer is already initialized.");
        }

        if (displayMode==DEFAULT_MODE) {
            //shader creation
            std::string vertexShaderName = "model.vert";
            if (fragmentShaderName.empty()) { //use default fragment shader
                fragmentShaderName = "model.frag";
            }
            createShader(vertexShaderName, geometryShaderName, fragmentShaderName);

            mNormalLoc = glGetUniformLocation(modelShader->getShaderId(), "mNormal");
            ambientFactorLoc = glGetUniformLocation(modelShader->getShaderId(), "ambientFactor");
            int diffuseTexLoc = glGetUniformLocation(modelShader->getShaderId(), "diffuseTex");
            int normalTexLoc = glGetUniformLocation(modelShader->getShaderId(), "normalTex");

            //activate texture
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(diffuseTexLoc, 0);
            glActiveTexture(GL_TEXTURE1);
            glUniform1i(normalTexLoc, 1);

            //setup mesh parameters
            meshParameter.setDiffuseTextureUnit(GL_TEXTURE0);
            meshParameter.setNormalTextureUnit(GL_TEXTURE1);
            meshParameter.setAmbientFactorLoc(ambientFactorLoc);
        } else if (displayMode==DEPTH_ONLY_MODE) {
            //shader creation
            std::string vertexShaderName = "modelDepthOnly.vert";
            if (fragmentShaderName.empty()) { //use default fragment shader
                fragmentShaderName = "modelDepthOnly.frag";
            }
            createShader(vertexShaderName, geometryShaderName, fragmentShaderName);

            mNormalLoc = 0;
            ambientFactorLoc = 0;

            //setup mesh parameters
            meshParameter.setDiffuseTextureUnit(-1);
            meshParameter.setNormalTextureUnit(-1);
            meshParameter.setAmbientFactorLoc(-1);
        } else {
            throw std::invalid_argument("Unknown display mode.");
        }

        //default matrix
        projectionMatrix = Matrix4<float>();
        modelShader->bind();
        glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float*)projectionMatrix);

        isInitialized=true;
    }

    void ModelDisplayer::createShader(const std::string &vertexShaderName, const std::string &geometryShaderName, const std::string &fragmentShaderName) {
        std::map<std::string, std::string> shaderTokens;
        shaderTokens.insert(fragmentTokens.begin(), fragmentTokens.end());
        shaderTokens.insert(geometryTokens.begin(), geometryTokens.end());

        modelShader = ShaderBuilder().createShader(vertexShaderName, geometryShaderName, fragmentShaderName, shaderTokens);
        modelShader->bind();

        mProjectionLoc = glGetUniformLocation(modelShader->getShaderId(), "mProjection");
        mModelLoc = glGetUniformLocation(modelShader->getShaderId(), "mModel");
        mViewLoc = glGetUniformLocation(modelShader->getShaderId(), "mView");
    }

    void ModelDisplayer::onCameraProjectionUpdate(const Camera *camera) {
        this->projectionMatrix = camera->getProjectionMatrix();

        modelShader->bind();
        glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float*)projectionMatrix);
    }

    int ModelDisplayer::getUniformLocation(const std::string &name) const {
        return glGetUniformLocation(modelShader->getShaderId(), name.c_str());
    }

    void ModelDisplayer::setCustomGeometryShader(const std::string &geometryShaderName, const std::map<std::string, std::string> &geometryTokens) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set custom geometry shader once the model displayer initialized.");
        }

        this->geometryShaderName = geometryShaderName;
        this->geometryTokens = geometryTokens;
    }

    void ModelDisplayer::setCustomFragmentShader(const std::string &fragmentShaderName, const std::map<std::string, std::string> &fragmentTokens) {
        if (isInitialized) {
            throw std::runtime_error("Impossible to set custom fragment shader once the model displayer initialized.");
        }

        this->fragmentShaderName = fragmentShaderName;
        this->fragmentTokens = fragmentTokens;
    }

    void ModelDisplayer::setCustomUniform(CustomUniform *customUniform) {
        this->customUniform = customUniform;
    }

    void ModelDisplayer::setCustomModelUniform(CustomModelUniform *customModelUniform) {
        this->customModelUniform = customModelUniform;
    }

    void ModelDisplayer::setModels(const std::vector<Model *> &models) {
        this->models = models;
    }

    void ModelDisplayer::updateAnimation(float dt) {
        ScopeProfiler profiler("3d", "updateAnimation");

        for (auto model : models) {
            model->updateAnimation(dt);
        }
    }

    void ModelDisplayer::display(const Matrix4<float> &viewMatrix) {
        ScopeProfiler profiler("3d", "modelDisplay");

        if (!isInitialized) {
            throw std::runtime_error("Model displayer must be initialized before display");
        }
        
        modelShader->bind();
        glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, (const float*)viewMatrix);
        if (customUniform) {
            customUniform->loadCustomUniforms();
        }

        for (const auto &model : models) {
            glUniformMatrix4fv(mModelLoc, 1, GL_FALSE, (const float*) model->getTransform().getTransformMatrix());
            if (displayMode==DEFAULT_MODE) {
                glUniformMatrix3fv(mNormalLoc, 1, GL_TRUE, (const float*) model->getTransform().getTransformMatrix().toMatrix3().inverse());
            }
            if (customModelUniform) {
                customModelUniform->loadCustomUniforms(model);
            }

            model->display(meshParameter);
        }
    }

    void ModelDisplayer::drawBBox(const Matrix4<float> &projectionMatrix, const Matrix4<float> &viewMatrix) const {
        for (auto model : models) {
            model->drawBBox(projectionMatrix, viewMatrix);
        }
    }

    void ModelDisplayer::drawBaseBones(const Matrix4<float> &projectionMatrix, const Matrix4<float> &viewMatrix, const std::string &meshFilename) const {
        for (auto model : models) {
            if (model->getMeshes() && model->getMeshes()->getMeshFilename()==meshFilename) {
                model->drawBaseBones(projectionMatrix, viewMatrix);
            }
        }
    }

}
