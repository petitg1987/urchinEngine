#include <memory>

#include <scene/renderer3d/model/displayer/ModelDisplayer.h>
#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <resources/material/UvScale.h>
#include <api/render/GenericRendererBuilder.h>

namespace urchin {

    ModelDisplayer::ModelDisplayer(const ModelSetDisplayer& modelSetDisplayer, Model& model, DisplayMode displayMode, RenderTarget& renderTarget, const Shader& shader) :
            isInitialized(false),
            modelSetDisplayer(modelSetDisplayer),
            instanceId(model.computeInstanceId(displayMode)),
            displayMode(displayMode),
            renderTarget(renderTarget),
            shader(shader),
            materialData({}),
            customShaderVariable(nullptr),
            depthTestEnabled(true),
            depthWriteEnabled(true),
            enableFaceCull(true) {
        model.attachModelDisplayer(*this);
    }

    ModelDisplayer::~ModelDisplayer() {
        //TODO add control to ensure instanceModels don't have reference to ModelDisplayer (and check in ~Model that ModelDisplayer has no ref too)

        for (Model* model : instanceModels) {
            model->removeObserver(this, Model::MATERIAL_UPDATED);
            model->removeObserver(this, Model::MESH_UPDATED);
            model->removeObserver(this, Model::SCALE_UPDATED);
        }
    }

    void ModelDisplayer::setupCustomShaderVariable(CustomModelShaderVariable* customShaderVariable) {
        if (customShaderVariable && isInitialized) {
            throw std::runtime_error("Can not define a custom model shader variable on an initialized model displayer: " + getReferenceModel().getConstMeshes()->getName());
        }
        this->customShaderVariable = customShaderVariable;
    }

    void ModelDisplayer::setupDepthOperations(bool depthTestEnabled, bool depthWriteEnabled) {
        if (isInitialized) {
            throw std::runtime_error("Can not define depth operations on an initialized model displayer: " + getReferenceModel().getConstMeshes()->getName());
        }
        this->depthTestEnabled = depthTestEnabled;
        this->depthWriteEnabled = depthWriteEnabled;
    }

    void ModelDisplayer::setupFaceCull(bool enableFaceCull) {
        if (isInitialized) {
            throw std::runtime_error("Can not define face cull flag on an initialized model displayer: " + getReferenceModel().getConstMeshes()->getName());
        }
        this->enableFaceCull = enableFaceCull;
    }

    void ModelDisplayer::setupBlendFunctions(const std::vector<BlendFunction>& blendFunctions) {
        if (!blendFunctions.empty() && isInitialized) {
            throw std::runtime_error("Can not define blend functions on an initialized model displayer: " + getReferenceModel().getConstMeshes()->getName());
        }
        this->blendFunctions = blendFunctions;
    }

    void ModelDisplayer::initialize() {
        if (isInitialized) {
            throw std::runtime_error("Model displayer is already initialized: " + getReferenceModel().getConstMeshes()->getName());
        }

        for (unsigned int i = 0; i < getReferenceModel().getMeshes()->getNumberMeshes(); ++i) {
            const ConstMesh& constMesh = getReferenceModel().getConstMeshes()->getConstMesh(i);
            const Mesh& mesh = getReferenceModel().getMeshes()->getMesh(i);
            auto meshName = getReferenceModel().getMeshes()->getConstMeshes().getMeshesName();

            InstanceMatrix identityInstanceMatrix;
            identityInstanceMatrix.modelMatrix = Matrix4<float>();
            identityInstanceMatrix.normalMatrix = Matrix4<float>();
            instanceMatrices.push_back(identityInstanceMatrix);
            Matrix4<float> projectionMatrix;
            fillMaterialData(mesh);

            auto meshRendererBuilder = GenericRendererBuilder::create("mesh - " + meshName, renderTarget, this->shader, ShapeType::TRIANGLE)
                    ->addData(mesh.getVertices())
                    ->instanceData(instanceMatrices.size(), VariableType::TWO_MAT4, (const float*)instanceMatrices.data())
                    ->indices(constMesh.getTrianglesIndices())
                    ->addUniformData(sizeof(projectionMatrix), &projectionMatrix) //binding 0
                    ->addUniformData(sizeof(materialData), &materialData); //binding 1 (only used in DEFAULT_MODE)

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
                        ->addData(uvScale.hasScaling() ? scaleUv(constMesh.getUvTexture(), uvScale) : constMesh.getUvTexture())
                        ->addData(mesh.getNormals())
                        ->addData(mesh.getTangents())
                        ->addUniformTextureReader(TextureReader::build(mesh.getMaterial().getDiffuseTexture(), buildTextureParam(mesh))) //binding 4
                        ->addUniformTextureReader(TextureReader::build(mesh.getMaterial().getNormalTexture(), buildTextureParam(mesh))); //binding 5
            }

            meshRenderers.push_back(meshRendererBuilder->build());
        }

        isInitialized = true;
    }

    bool ModelDisplayer::hasInstancing() const {
        return instanceModels.size() > 1;
    }

    Model& ModelDisplayer::getReferenceModel() const {
        //A reference model is a model which can be used to represent all instance models.
        //For unique properties (e.g. Model#getTransform()#getPosition()): do not use the reference model.
        if (instanceModels.empty()) {
            throw std::runtime_error("No reference model on bared model displayer");
        }
        return *instanceModels[0];
    }

    void ModelDisplayer::fillMaterialData(const Mesh& mesh) {
        materialData.encodedEmissiveFactor = MathFunction::clamp(mesh.getMaterial().getEmissiveFactor() / Material::MAX_EMISSIVE_FACTOR, 0.0f, 1.0f);
        materialData.ambientFactor = mesh.getMaterial().getAmbientFactor();
    }

    std::vector<Point2<float>> ModelDisplayer::scaleUv(const std::vector<Point2<float>>& uvTexture, const UvScale& uvScale) const {
        std::vector<Point2<float>> scaledUvTexture;
        scaledUvTexture.reserve(uvTexture.size());

        const Vector3<float> scale = getReferenceModel().getTransform().getScale();
        for (const Point2<float>& uv : uvTexture) {
            scaledUvTexture.emplace_back(uvScale.scaleU(uv.X, scale), uvScale.scaleV(uv.Y, scale));
        }
        return scaledUvTexture;
    }

    TextureParam ModelDisplayer::buildTextureParam(const Mesh& mesh) const {
        TextureParam::ReadMode textureReadMode = mesh.getMaterial().repeatTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
        return TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);
    }

    void ModelDisplayer::notify(Observable* observable, int notificationType) {
        if (const auto* model = dynamic_cast<Model*>(observable)) {
            if (notificationType == Model::MESH_UPDATED) {
                assert(!hasInstancing()); //The mesh cannot be updated on a model displayer using instancing. A new model displayer should be used.
                unsigned int meshIndex = 0;
                for (const auto& meshRenderer : meshRenderers) {
                    const Mesh& mesh = model->getMeshes()->getMesh(meshIndex);
                    meshRenderer->updateData(0, mesh.getVertices());
                    if (displayMode == DisplayMode::DEFAULT_MODE) {
                        meshRenderer->updateData(2, mesh.getNormals());
                        meshRenderer->updateData(3, mesh.getTangents());
                    }

                    meshIndex++;
                }
            } else if (notificationType == Model::MATERIAL_UPDATED) {
                if (displayMode == DisplayMode::DEFAULT_MODE) {
                    assert(!hasInstancing()); //The material cannot be updated on a model displayer using instancing. A new model displayer should be used.
                    unsigned int meshIndex = 0;
                    for (const auto& meshRenderer : meshRenderers) {
                        const Mesh& mesh = model->getMeshes()->getMesh(meshIndex);

                        fillMaterialData(mesh);
                        meshRenderer->updateUniformData(1, &materialData);

                        if (meshRenderer->getUniformTextureReader(0)->getTexture() != mesh.getMaterial().getDiffuseTexture().get()) {
                            meshRenderer->updateUniformTextureReader(0, TextureReader::build(mesh.getMaterial().getDiffuseTexture(), buildTextureParam(mesh)));
                        }
                        if (meshRenderer->getUniformTextureReader(1)->getTexture() != mesh.getMaterial().getNormalTexture().get()) {
                            meshRenderer->updateUniformTextureReader(1, TextureReader::build(mesh.getMaterial().getNormalTexture(), buildTextureParam(mesh)));
                        }

                        meshIndex++;
                    }
                }
            } else if (notificationType == Model::SCALE_UPDATED) {
                if (displayMode == DisplayMode::DEFAULT_MODE) {
                    assert(!hasInstancing()); //The scale cannot be updated on a model displayer using instancing. A new model displayer should be used.
                    unsigned int meshIndex = 0;
                    for (const auto& meshRenderer: meshRenderers) {
                        const ConstMesh& constMesh = model->getConstMeshes()->getConstMesh(meshIndex);
                        const Mesh& mesh = model->getMeshes()->getMesh(meshIndex);
                        const UvScale& uvScale = mesh.getMaterial().getUvScale();
                        meshRenderer->updateData(1, uvScale.hasScaling() ? scaleUv(constMesh.getUvTexture(), uvScale) : constMesh.getUvTexture());

                        meshIndex++;
                    }
                }
            }
        }
    }

    const ModelSetDisplayer& ModelDisplayer::getModelSetDisplayer() const {
        return modelSetDisplayer;
    }

    std::size_t ModelDisplayer::getInstanceId() const {
        return instanceId;
    }

    void ModelDisplayer::addInstanceModel(Model& model) {
        #ifdef URCHIN_DEBUG
            assert(instanceModels.empty() || instanceId != ModelDisplayable::INSTANCING_DENY_ID);
            assert(instanceId == model.computeInstanceId(displayMode));
        #endif
        instanceModels.push_back(&model);

        model.addObserver(this, Model::MESH_UPDATED);
        model.addObserver(this, Model::MATERIAL_UPDATED);
        model.addObserver(this, Model::SCALE_UPDATED);
    }

    void ModelDisplayer::removeInstanceModel(Model& modelToRemove) {
        std::size_t erasedCount = std::erase_if(instanceModels, [&modelToRemove](const Model* model) {return model == &modelToRemove;});
        if (erasedCount != 1) {
            throw std::runtime_error("Removing the instance model fail: " + modelToRemove.getConstMeshes()->getId());
        }

        modelToRemove.removeObserver(this, Model::MATERIAL_UPDATED);
        modelToRemove.removeObserver(this, Model::MESH_UPDATED);
        modelToRemove.removeObserver(this, Model::SCALE_UPDATED);
    }

    unsigned int ModelDisplayer::getInstanceCount() const {
        return (unsigned int)instanceModels.size();
    }

    void ModelDisplayer::prepareRendering(unsigned int renderingOrder, const Matrix4<float>& projectionViewMatrix, const MeshFilter* meshFilter) const {
        unsigned int meshIndex = 0;
        for (auto& meshRenderer : meshRenderers) {
            if (meshRenderer->isEnabled()) {
                continue; //TODO find better solution
            }

            const Mesh& mesh = getReferenceModel().getMeshes()->getMesh(meshIndex++);
            if (meshFilter && !meshFilter->isAccepted(mesh)) {
                continue;
            }

            instanceMatrices.clear();
            for (Model* instanceModel : instanceModels) {
                InstanceMatrix instanceMatrix;
                instanceMatrix.modelMatrix = instanceModel->getTransform().getTransformMatrix();
                instanceMatrix.normalMatrix = instanceModel->getTransform().getTransformMatrix().inverse().transpose();
                instanceMatrices.push_back(instanceMatrix);
            }
            meshRenderer->updateInstanceData(instanceMatrices.size(), (const float*)instanceMatrices.data());

            meshRenderer->updateUniformData(0, &projectionViewMatrix);
            if (customShaderVariable) {
                customShaderVariable->loadCustomShaderVariables(*meshRenderer);
            }

            meshRenderer->enableRenderer(renderingOrder);
        }
    }

    void ModelDisplayer::drawBBox(GeometryContainer& geometryContainer) {
        for (auto& aabboxModel : aabboxModels) {
            geometryContainer.removeGeometry(*aabboxModel);
        }
        aabboxModels.clear();

        for (Model* instanceModel : instanceModels) {
            aabboxModels.push_back(std::make_shared<AABBoxModel>(instanceModel->getAABBox()));
            geometryContainer.addGeometry(aabboxModels.back());
        }
    }

    void ModelDisplayer::drawBaseBones(GeometryContainer& geometryContainer, const MeshFilter* meshFilter) const {
        for (Model* instanceModel : instanceModels) {
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
