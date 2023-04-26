#include <memory>

#include <scene/renderer3d/model/displayer/ModelInstanceDisplayer.h>
#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <resources/material/UvScale.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    ModelInstanceDisplayer::ModelInstanceDisplayer(const ModelSetDisplayer& modelSetDisplayer, DisplayMode displayMode, RenderTarget& renderTarget, const Shader& shader) :
            isInitialized(false),
            modelSetDisplayer(modelSetDisplayer),
            instanceId(ModelDisplayable::INSTANCING_DENY_ID),
            displayMode(displayMode),
            renderTarget(renderTarget),
            shader(shader),
            materialData({}),
            customShaderVariable(nullptr),
            depthTestEnabled(true),
            depthWriteEnabled(true),
            enableFaceCull(true) {

    }

    ModelInstanceDisplayer::~ModelInstanceDisplayer() {
        std::vector<Model*> copiedInstanceModels = instanceModels;
        for (Model* model : copiedInstanceModels) {
            removeInstanceModel(*model);
        }
    }

    void ModelInstanceDisplayer::setupCustomShaderVariable(CustomModelShaderVariable* customShaderVariable) {
        if (customShaderVariable && isInitialized) {
            throw std::runtime_error("Can not define a custom model shader variable on an initialized model displayer: " + getReferenceModel().getConstMeshes()->getName());
        }
        this->customShaderVariable = customShaderVariable;
    }

    void ModelInstanceDisplayer::setupDepthOperations(bool depthTestEnabled, bool depthWriteEnabled) {
        if (isInitialized) {
            throw std::runtime_error("Can not define depth operations on an initialized model displayer: " + getReferenceModel().getConstMeshes()->getName());
        }
        this->depthTestEnabled = depthTestEnabled;
        this->depthWriteEnabled = depthWriteEnabled;
    }

    void ModelInstanceDisplayer::setupFaceCull(bool enableFaceCull) {
        if (isInitialized) {
            throw std::runtime_error("Can not define face cull flag on an initialized model displayer: " + getReferenceModel().getConstMeshes()->getName());
        }
        this->enableFaceCull = enableFaceCull;
    }

    void ModelInstanceDisplayer::setupBlendFunctions(const std::vector<BlendFunction>& blendFunctions) {
        if (!blendFunctions.empty() && isInitialized) {
            throw std::runtime_error("Can not define blend functions on an initialized model displayer: " + getReferenceModel().getConstMeshes()->getName());
        }
        this->blendFunctions = blendFunctions;
    }

    void ModelInstanceDisplayer::initialize() {
        if (isInitialized) {
            throw std::runtime_error("Model displayer is already initialized: " + getReferenceModel().getConstMeshes()->getName());
        } else if (instanceModels.empty()) {
            throw std::runtime_error("At least one instance model must be added before initialization");
        }

        for (unsigned int i = 0; i < getReferenceModel().getMeshes()->getNumberMeshes(); ++i) {
            const ConstMesh& constMesh = getReferenceModel().getConstMeshes()->getConstMesh(i);
            const Mesh& mesh = getReferenceModel().getMeshes()->getMesh(i);
            auto meshName = getReferenceModel().getMeshes()->getConstMeshes().getMeshesName();

            Matrix4<float> projectionMatrix;
            fillMaterialData(mesh);

            auto meshRendererBuilder = GenericRendererBuilder::create("mesh - " + meshName, renderTarget, this->shader, ShapeType::TRIANGLE)
                    ->addData(mesh.getVertices())
                    ->indices(constMesh.getTrianglesIndices())
                    ->addUniformData(sizeof(projectionMatrix), &projectionMatrix) //binding 0
                    ->addUniformData(sizeof(materialData), &materialData); //binding 1 (only used in DEFAULT_MODE)

            if (displayMode == DisplayMode::DEFAULT_MODE) {
                instanceMatrices.emplace_back(InstanceMatrix{.modelMatrix = Matrix4<float>(), .normalMatrix = Matrix4<float>()});
                meshRendererBuilder->instanceData(instanceMatrices.size(), VariableType::TWO_MAT4, (const float*)instanceMatrices.data());
            } else if (displayMode == DisplayMode::DEPTH_ONLY_MODE) {
                instanceModelMatrices.emplace_back();
                meshRendererBuilder->instanceData(instanceModelMatrices.size(), VariableType::MAT4, (const float*)instanceModelMatrices.data());
            }

            if (customShaderVariable) {
                customShaderVariable->setupMeshRenderer(meshRendererBuilder); //binding 2 & 3 (optional)
            }
            int missingUniformData = 4 - (int)meshRendererBuilder->getUniformData().size();
            assert(missingUniformData >= 0);
            for (int i = 0; i < missingUniformData; ++i) {
                int customDummyValue = 0;
                meshRendererBuilder->addUniformData(sizeof(customDummyValue), &customDummyValue); //binding 2 & 3
            }

            if (depthTestEnabled && mesh.getMaterial().isDepthTestEnabled()) {
                meshRendererBuilder->enableDepthTest();
            }
            if (depthWriteEnabled && mesh.getMaterial().isDepthWriteEnabled()) {
                meshRendererBuilder->enableDepthWrite();
            }
            if (!enableFaceCull) {
                meshRendererBuilder->disableCullFace();
            }
            if (!blendFunctions.empty()) {
                meshRendererBuilder->enableTransparency(blendFunctions);
            }

            if (displayMode == DisplayMode::DEFAULT_MODE) {
                const UvScale& uvScale = mesh.getMaterial().getUvScale();
                meshRendererBuilder
                        ->addData(uvScale.hasScaling() ? scaleUv(mesh.getUv(), mesh.getNormals(), uvScale) : mesh.getUv())
                        ->addData(mesh.getNormals())
                        ->addData(mesh.getTangents())
                        ->addUniformTextureReader(TextureReader::build(mesh.getMaterial().getAlbedoTexture(), buildTextureParam(mesh))) //binding 4
                        ->addUniformTextureReader(TextureReader::build(mesh.getMaterial().getNormalTexture(), buildTextureParam(mesh))) //binding 5
                        ->addUniformTextureReader(TextureReader::build(mesh.getMaterial().getRoughnessTexture(), buildTextureParam(mesh))) //binding 6
                        ->addUniformTextureReader(TextureReader::build(mesh.getMaterial().getMetalnessTexture(), buildTextureParam(mesh))); //binding 7
            }

            meshRenderers.push_back(meshRendererBuilder->build());
        }

        isInitialized = true;
    }

    void ModelInstanceDisplayer::updateMeshVertices(std::size_t instanceId, const Model* model) {
        this->instanceId = instanceId;

        unsigned int meshIndex = 0;
        for (const auto& meshRenderer: meshRenderers) {
            if (model->isMeshUpdated(meshIndex)) {
                const Mesh& mesh = getReferenceModel().getMeshes()->getMesh(meshIndex);
                meshRenderer->updateData(0, mesh.getVertices());
                if (displayMode == DisplayMode::DEFAULT_MODE) {
                    meshRenderer->updateData(2, mesh.getNormals());
                    meshRenderer->updateData(3, mesh.getTangents());
                }
            }
            meshIndex++;
        }
    }

    void ModelInstanceDisplayer::updateMeshUv(std::size_t instanceId, const Model* model) {
        this->instanceId = instanceId;

        unsigned int meshIndex = 0;
        for (const auto& meshRenderer : meshRenderers) {
            if (model->isMeshUpdated(meshIndex)) {
                const Mesh& mesh = getReferenceModel().getMeshes()->getMesh(meshIndex);
                if (displayMode == DisplayMode::DEFAULT_MODE) {
                    meshRenderer->updateData(1, mesh.getUv());
                }
            }
            meshIndex++;
        }
    }

    void ModelInstanceDisplayer::updateMaterial(std::size_t instanceId, const Model* model) {
        this->instanceId = instanceId;

        if (displayMode == DisplayMode::DEFAULT_MODE) {
            unsigned int meshIndex = 0;
            for (const auto& meshRenderer : meshRenderers) {
                if (model->isMeshUpdated(meshIndex)) {
                    const Mesh& mesh = getReferenceModel().getMeshes()->getMesh(meshIndex);
                    fillMaterialData(mesh);
                    meshRenderer->updateUniformData(1, &materialData);

                    if (meshRenderer->getUniformTextureReader(0)->getTexture() != mesh.getMaterial().getAlbedoTexture().get()) {
                        meshRenderer->updateUniformTextureReader(0, TextureReader::build(mesh.getMaterial().getAlbedoTexture(), buildTextureParam(mesh)));
                    }
                    if (meshRenderer->getUniformTextureReader(1)->getTexture() != mesh.getMaterial().getNormalTexture().get()) {
                        meshRenderer->updateUniformTextureReader(1, TextureReader::build(mesh.getMaterial().getNormalTexture(), buildTextureParam(mesh)));
                    }
                    if (meshRenderer->getUniformTextureReader(2)->getTexture() != mesh.getMaterial().getRoughnessTexture().get()) {
                        meshRenderer->updateUniformTextureReader(2, TextureReader::build(mesh.getMaterial().getRoughnessTexture(), buildTextureParam(mesh)));
                    }
                    if (meshRenderer->getUniformTextureReader(3)->getTexture() != mesh.getMaterial().getMetalnessTexture().get()) {
                        meshRenderer->updateUniformTextureReader(3, TextureReader::build(mesh.getMaterial().getMetalnessTexture(), buildTextureParam(mesh)));
                    }
                }
                meshIndex++;
            }
        }
    }

    void ModelInstanceDisplayer::updateScale(std::size_t instanceId) {
        this->instanceId = instanceId;

        if (displayMode == DisplayMode::DEFAULT_MODE) {
            unsigned int meshIndex = 0;
            for (const auto& meshRenderer: meshRenderers) {
                const Mesh& mesh = getReferenceModel().getMeshes()->getMesh(meshIndex);
                const UvScale& uvScale = mesh.getMaterial().getUvScale();
                meshRenderer->updateData(1, uvScale.hasScaling() ? scaleUv(mesh.getUv(), mesh.getNormals(), uvScale) : mesh.getUv());

                meshIndex++;
            }
        }
    }

    Model& ModelInstanceDisplayer::getReferenceModel() const {
        //A reference model is a model which can be used to represent all instance models.
        //For unique properties (e.g. Model#getTransform()#getPosition()): do not use the reference model.
        if (instanceModels.empty()) {
            throw std::runtime_error("No reference model on bared model displayer");
        }
        return *instanceModels[0];
    }

    void ModelInstanceDisplayer::fillMaterialData(const Mesh& mesh) {
        materialData.encodedEmissiveFactor = std::clamp(mesh.getMaterial().getEmissiveFactor() / Material::MAX_EMISSIVE_FACTOR, 0.0f, 1.0f);
        materialData.ambientFactor = mesh.getMaterial().getAmbientFactor();
    }

    std::vector<Point2<float>> ModelInstanceDisplayer::scaleUv(const std::vector<Point2<float>>& uvTexture, const std::vector<Vector3<float>>& normals, const UvScale& uvScale) const {
        assert(uvTexture.size() == normals.size());
        std::vector<Point2<float>> scaledUv;
        scaledUv.reserve(uvTexture.size());

        const Vector3<float>& modelScale = getReferenceModel().getTransform().getScale();
        for (std::size_t vertexIndex = 0; vertexIndex < uvTexture.size(); ++vertexIndex) {
            scaledUv.emplace_back(uvScale.scaleUV(uvTexture[vertexIndex], modelScale, normals[vertexIndex]));
        }
        return scaledUv;
    }

    TextureParam ModelInstanceDisplayer::buildTextureParam(const Mesh& mesh) const {
        TextureParam::ReadMode textureReadMode = mesh.getMaterial().repeatTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
        return TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);
    }

    const ModelSetDisplayer& ModelInstanceDisplayer::getModelSetDisplayer() const {
        return modelSetDisplayer;
    }

    std::size_t ModelInstanceDisplayer::getInstanceId() const {
        return instanceId;
    }

    const std::vector<Model*>& ModelInstanceDisplayer::getInstanceModels() const {
        return instanceModels;
    }

    void ModelInstanceDisplayer::addInstanceModel(Model& model) {
        if (instanceModels.empty()) {
            instanceId = model.computeInstanceId(displayMode);
        } else {
            #ifdef URCHIN_DEBUG
                assert(instanceId != ModelDisplayable::INSTANCING_DENY_ID);
                assert(instanceId == model.computeInstanceId(displayMode));
            #endif
        }

        instanceModels.push_back(&model);
        model.attachModelInstanceDisplayer(*this);
    }

    void ModelInstanceDisplayer::removeInstanceModel(Model& model) {
        model.detachModelInstanceDisplayer(*this);
        std::size_t erasedCount = std::erase_if(instanceModels, [&model](const Model* m) {return m == &model;});
        if (erasedCount != 1) {
            Logger::instance().logError("Removing the instance model fail: " + model.getConstMeshes()->getId());
        }
    }

    unsigned int ModelInstanceDisplayer::getInstanceCount() const {
        return (unsigned int)instanceModels.size();
    }

    void ModelInstanceDisplayer::resetRenderingModels() {
        instanceMatrices.clear();
        instanceModelMatrices.clear();
    }

    void ModelInstanceDisplayer::registerRenderingModel(const Model& model) {
        #ifdef URCHIN_DEBUG
            assert(model.computeInstanceId(displayMode) == instanceId);
        #endif

        if (displayMode == DisplayMode::DEFAULT_MODE) {
            InstanceMatrix instanceMatrix;
            instanceMatrix.modelMatrix = model.getTransform().getTransformMatrix();
            instanceMatrix.normalMatrix = instanceMatrix.modelMatrix.inverse().transpose();
            instanceMatrices.push_back(instanceMatrix);
        } else if (displayMode == DisplayMode::DEPTH_ONLY_MODE) {
            instanceModelMatrices.push_back(model.getTransform().getTransformMatrix());
        }
    }

    void ModelInstanceDisplayer::prepareRendering(unsigned int renderingOrder, const Matrix4<float>& projectionViewMatrix, const MeshFilter* meshFilter) const {
        if (instanceMatrices.empty() && instanceModelMatrices.empty()) {
            return;
        }

        unsigned int meshIndex = 0;
        for (auto& meshRenderer : meshRenderers) {
            const Mesh& mesh = getReferenceModel().getMeshes()->getMesh(meshIndex++);
            if (meshFilter && !meshFilter->isAccepted(mesh)) {
                continue;
            }

            if (displayMode == DisplayMode::DEFAULT_MODE) {
                meshRenderer->updateInstanceData(instanceMatrices.size(), (const float*) instanceMatrices.data());
            } else if (displayMode == DisplayMode::DEPTH_ONLY_MODE) {
                meshRenderer->updateInstanceData(instanceModelMatrices.size(), (const float*) instanceModelMatrices.data());
            }
            meshRenderer->updateUniformData(0, &projectionViewMatrix);
            if (customShaderVariable) {
                customShaderVariable->loadCustomShaderVariables(*meshRenderer);
            }

            meshRenderer->enableRenderer(renderingOrder);
        }
    }

    void ModelInstanceDisplayer::drawBBox(GeometryContainer& geometryContainer) {
        for (const auto& aabboxModel : aabboxModels) {
            geometryContainer.removeGeometry(*aabboxModel);
        }
        aabboxModels.clear();

        for (const Model* instanceModel : instanceModels) {
            aabboxModels.push_back(std::make_shared<AABBoxModel>(instanceModel->getAABBox()));
            geometryContainer.addGeometry(aabboxModels.back());
        }
    }

    void ModelInstanceDisplayer::drawBaseBones(GeometryContainer& geometryContainer, const MeshFilter* meshFilter) const {
        for (const Model* instanceModel : instanceModels) {
            if (instanceModel->getMeshes()) {
                for (unsigned int m = 0; m < instanceModel->getMeshes()->getNumberMeshes(); ++m) {
                    const Mesh& mesh = instanceModel->getMeshes()->getMesh(m);
                    if (!meshFilter || meshFilter->isAccepted(mesh)) {
                        instanceModel->getMeshes()->getMesh(m).drawBaseBones(geometryContainer, instanceModel->getTransform().getTransformMatrix());
                    }
                }
            }
        }
    }

}
