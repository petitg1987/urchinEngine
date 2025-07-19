#include <memory>
#include <cstring>

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
            enableFaceCull(true),
            enableLayerIndexDataInShader(false),
            layersMask(ULLONG_MAX) {
        std::memset(&materialData, 0, sizeof(materialData));
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

    void ModelInstanceDisplayer::setupLayerIndexDataInShader(bool enableLayerIndexDataInShader) {
        if (isInitialized) {
            throw std::runtime_error("Can not define layer index data flag on an initialized model displayer: " + getReferenceModel().getConstMeshes()->getName());
        }
        this->enableLayerIndexDataInShader = enableLayerIndexDataInShader;
    }

    void ModelInstanceDisplayer::setupLayersMask(std::bitset<8> layersMask) {
        if (isInitialized) {
            throw std::runtime_error("Can not define a layer mask on an initialized model displayer: " + getReferenceModel().getConstMeshes()->getName());
        }
        this->layersMask = layersMask;
    }

    void ModelInstanceDisplayer::setupCustomTextures(const std::array<std::shared_ptr<TextureReader>, 2>& textureReaders) {
        if (isInitialized) {
            throw std::runtime_error("Can not define a custom texture on an initialized model displayer: " + getReferenceModel().getConstMeshes()->getName());
        }
        this->textureReaders = textureReaders;
    }

    void ModelInstanceDisplayer::initialize() {
        if (isInitialized) {
            throw std::runtime_error("Model displayer is already initialized: " + getReferenceModel().getConstMeshes()->getName());
        } else if (instanceModels.empty()) {
            throw std::runtime_error("At least one instance model must be added before initialization");
        }

        for (unsigned int i = 0; i < getReferenceModel().getMeshes()->getNumMeshes(); ++i) {
            const ConstMesh& constMesh = getReferenceModel().getConstMeshes()->getConstMesh(i);
            const Mesh& mesh = getReferenceModel().getMeshes()->getMesh(i);
            auto meshName = getReferenceModel().getMeshes()->getConstMeshes().getMeshesName();

            Matrix4<float> projectionViewMatrix;
            fillMaterialData(mesh);

            auto meshRendererBuilder = GenericRendererBuilder::create("mesh - " + meshName, renderTarget, this->shader, ShapeType::TRIANGLE)
                    ->addData(mesh.getVertices())
                    ->indices(std::span(reinterpret_cast<const unsigned int*>(constMesh.getTrianglesIndices().data()), constMesh.getTrianglesIndices().size() * 3))
                    ->addUniformData(PROJ_VIEW_MATRIX_UNIFORM_BINDING, sizeof(projectionViewMatrix), &projectionViewMatrix)
                    ->addUniformData(MAT_DATA_UNIFORM_BINDING, sizeof(materialData), &materialData); //only used in DEFAULT_MODE

            if (displayMode == DisplayMode::DEFAULT_MODE || displayMode == DisplayMode::DEFAULT_NO_INSTANCING_MODE) {
                instanceMatrices.emplace_back(InstanceMatrix{.modelMatrix = Matrix4<float>(), .normalMatrix = Matrix4<float>()});
                meshRendererBuilder->instanceData(instanceMatrices.size(), {VariableType::MAT4_FLOAT, VariableType::MAT4_FLOAT}, (const float*)instanceMatrices.data());
            } else if (displayMode == DisplayMode::DEPTH_ONLY_MODE) {
                instanceModelMatrices.emplace_back();
                meshRendererBuilder->instanceData(instanceModelMatrices.size(), {VariableType::MAT4_FLOAT}, (const float*)instanceModelMatrices.data());
            }

            if (customShaderVariable) {
                customShaderVariable->setupMeshRenderer(meshRendererBuilder, CUSTOM1_UNIFORM_BINDING, CUSTOM2_UNIFORM_BINDING);
            }
            int customDummyValue = 0;
            if (!meshRendererBuilder->getUniformData().contains(CUSTOM1_UNIFORM_BINDING)) {
                meshRendererBuilder->addUniformData(CUSTOM1_UNIFORM_BINDING, sizeof(customDummyValue), &customDummyValue);
            }
            if (!meshRendererBuilder->getUniformData().contains(CUSTOM2_UNIFORM_BINDING)) {
                meshRendererBuilder->addUniformData(CUSTOM2_UNIFORM_BINDING, sizeof(customDummyValue), &customDummyValue);
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
            if (enableLayerIndexDataInShader) {
                meshRendererBuilder->enableLayerIndexDataInShader();
            }
            if (!layersMask.all()) {
                meshRendererBuilder->setLayersMask(layersMask);
            }

            if (displayMode == DisplayMode::DEFAULT_MODE || displayMode == DisplayMode::DEFAULT_NO_INSTANCING_MODE) {
                const UvScale& uvScale = mesh.getMaterial().getUvScale();
                meshRendererBuilder
                        ->addData(uvScale.hasScaling() ? scaleUv(mesh.getUv(), mesh.getNormals(), uvScale) : mesh.getUv())
                        ->addData(mesh.getNormals())
                        ->addData(mesh.getTangents())
                        ->addUniformTextureReader(MAT_ALBEDO_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getAlbedoTexture(), buildTextureParam(mesh)))
                        ->addUniformTextureReader(MAT_NORMAL_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getNormalTexture(), buildTextureParam(mesh)))
                        ->addUniformTextureReader(MAT_ROUGHNESS_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getRoughnessTexture(), buildTextureParam(mesh)))
                        ->addUniformTextureReader(MAT_METALNESS_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getMetalnessTexture(), buildTextureParam(mesh)));

                if (textureReaders[0]) {
                    meshRendererBuilder->addUniformTextureReader(CUSTOM1_TEX_UNIFORM_BINDING, textureReaders[0]);
                }
                if (textureReaders[1]) {
                    meshRendererBuilder->addUniformTextureReader(CUSTOM2_TEX_UNIFORM_BINDING, textureReaders[1]);
                }
            }

            meshRenderers.push_back(meshRendererBuilder->build());
        }

        isInitialized = true;
    }

    void ModelInstanceDisplayer::updateLayersMask(std::bitset<8> layersMask) {
        this->layersMask = layersMask;
        for (auto& meshRenderer : meshRenderers) {
            meshRenderer->updateLayersMask(layersMask);
        }
    }

    std::bitset<8> ModelInstanceDisplayer::getLayersMask() const {
        return layersMask;
    }

    void ModelInstanceDisplayer::updateMeshVertices(const Model* model) const {
        unsigned int meshIndex = 0;
        for (const auto& meshRenderer: meshRenderers) {
            if (model->isMeshUpdated(meshIndex)) {
                const Mesh& mesh = getReferenceModel().getMeshes()->getMesh(meshIndex);
                meshRenderer->updateData(0, mesh.getVertices());
                if (displayMode == DisplayMode::DEFAULT_MODE || displayMode == DisplayMode::DEFAULT_NO_INSTANCING_MODE) {
                    meshRenderer->updateData(2, mesh.getNormals());
                    meshRenderer->updateData(3, mesh.getTangents());
                }
            }
            meshIndex++;
        }
    }

    void ModelInstanceDisplayer::updateMeshUv(const Model* model) const {
        unsigned int meshIndex = 0;
        for (const auto& meshRenderer : meshRenderers) {
            if (model->isMeshUpdated(meshIndex)) {
                const Mesh& mesh = getReferenceModel().getMeshes()->getMesh(meshIndex);
                if (displayMode == DisplayMode::DEFAULT_MODE || displayMode == DisplayMode::DEFAULT_NO_INSTANCING_MODE) {
                    meshRenderer->updateData(1, mesh.getUv());
                }
            }
            meshIndex++;
        }
    }

    void ModelInstanceDisplayer::updateMaterial(const Model* model) const {
        if (displayMode == DisplayMode::DEFAULT_MODE || displayMode == DisplayMode::DEFAULT_NO_INSTANCING_MODE) {
            unsigned int meshIndex = 0;
            for (const auto& meshRenderer : meshRenderers) {
                if (model->isMeshUpdated(meshIndex)) {
                    const Mesh& mesh = getReferenceModel().getMeshes()->getMesh(meshIndex);
                    fillMaterialData(mesh);
                    meshRenderer->updateUniformData(MAT_DATA_UNIFORM_BINDING, &materialData);

                    if (meshRenderer->getUniformTextureReader(MAT_ALBEDO_UNIFORM_BINDING)->getTexture() != mesh.getMaterial().getAlbedoTexture().get()) {
                        meshRenderer->updateUniformTextureReader(MAT_ALBEDO_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getAlbedoTexture(), buildTextureParam(mesh)));
                    }
                    if (meshRenderer->getUniformTextureReader(MAT_NORMAL_UNIFORM_BINDING)->getTexture() != mesh.getMaterial().getNormalTexture().get()) {
                        meshRenderer->updateUniformTextureReader(MAT_NORMAL_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getNormalTexture(), buildTextureParam(mesh)));
                    }
                    if (meshRenderer->getUniformTextureReader(MAT_ROUGHNESS_UNIFORM_BINDING)->getTexture() != mesh.getMaterial().getRoughnessTexture().get()) {
                        meshRenderer->updateUniformTextureReader(MAT_ROUGHNESS_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getRoughnessTexture(), buildTextureParam(mesh)));
                    }
                    if (meshRenderer->getUniformTextureReader(MAT_METALNESS_UNIFORM_BINDING)->getTexture() != mesh.getMaterial().getMetalnessTexture().get()) {
                        meshRenderer->updateUniformTextureReader(MAT_METALNESS_UNIFORM_BINDING, TextureReader::build(mesh.getMaterial().getMetalnessTexture(), buildTextureParam(mesh)));
                    }
                }
                meshIndex++;
            }
        }
    }

    void ModelInstanceDisplayer::updateScale() const {
        if (displayMode == DisplayMode::DEFAULT_MODE || displayMode == DisplayMode::DEFAULT_NO_INSTANCING_MODE) {
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

    void ModelInstanceDisplayer::fillMaterialData(const Mesh& mesh) const {
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

    void ModelInstanceDisplayer::updateInstanceId(std::size_t instanceId) {
        this->instanceId = instanceId;
    }

    std::span<Model* const> ModelInstanceDisplayer::getInstanceModels() const {
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

    void ModelInstanceDisplayer::resetRenderingModels() const {
        instanceMatrices.clear();
        instanceModelMatrices.clear();
    }

    void ModelInstanceDisplayer::registerRenderingModel(const Model& model) const {
        #ifdef URCHIN_DEBUG
            assert(model.computeInstanceId(displayMode) == instanceId);
        #endif

        if (displayMode == DisplayMode::DEFAULT_MODE || displayMode == DisplayMode::DEFAULT_NO_INSTANCING_MODE) {
            InstanceMatrix instanceMatrix;
            instanceMatrix.modelMatrix = model.getTransform().getTransformMatrix();
            instanceMatrix.normalMatrix = instanceMatrix.modelMatrix.inverse().transpose();
            instanceMatrices.push_back(instanceMatrix);
        } else if (displayMode == DisplayMode::DEPTH_ONLY_MODE) {
            instanceModelMatrices.push_back(model.getTransform().getTransformMatrix());
        }
    }

    void ModelInstanceDisplayer::prepareRendering(uint32_t /*frameCount*/, unsigned int renderingOrder, const Matrix4<float>& projectionViewMatrix, const MeshFilter* meshFilter) const {
        if (instanceMatrices.empty() && instanceModelMatrices.empty()) {
            return;
        }

        unsigned int meshIndex = 0;
        for (auto& meshRenderer : meshRenderers) {
            const Mesh& mesh = getReferenceModel().getMeshes()->getMesh(meshIndex++);
            if (meshFilter && !meshFilter->isAccepted(mesh)) {
                continue;
            }

            if (displayMode == DisplayMode::DEFAULT_MODE || displayMode == DisplayMode::DEFAULT_NO_INSTANCING_MODE) {
                meshRenderer->updateInstanceData(instanceMatrices.size(), (const float*) instanceMatrices.data());
            } else if (displayMode == DisplayMode::DEPTH_ONLY_MODE) {
                meshRenderer->updateInstanceData(instanceModelMatrices.size(), (const float*) instanceModelMatrices.data());
            }
            meshRenderer->updateUniformData(PROJ_VIEW_MATRIX_UNIFORM_BINDING, &projectionViewMatrix);
            if (customShaderVariable) {
                customShaderVariable->loadCustomShaderVariables(*meshRenderer, CUSTOM1_UNIFORM_BINDING, CUSTOM2_UNIFORM_BINDING);
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
            aabboxModels.back()->setPolygonMode(PolygonMode::WIREFRAME);
            geometryContainer.addGeometry(aabboxModels.back());
        }
    }

    void ModelInstanceDisplayer::drawBaseBones(GeometryContainer& geometryContainer, const MeshFilter* meshFilter) const {
        for (const Model* instanceModel : instanceModels) {
            if (instanceModel->getMeshes()) {
                for (unsigned int m = 0; m < instanceModel->getMeshes()->getNumMeshes(); ++m) {
                    const Mesh& mesh = instanceModel->getMeshes()->getMesh(m);
                    if (!meshFilter || meshFilter->isAccepted(mesh)) {
                        instanceModel->getMeshes()->getMesh(m).drawBaseBones(geometryContainer, instanceModel->getTransform().getTransformMatrix());
                    }
                }
            }
        }
    }

}
