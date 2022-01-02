#include <memory>

#include <scene/renderer3d/model/displayer/ModelInstanceDisplayer.h>
#include <scene/renderer3d/model/displayer/ModelSetDisplayer.h>
#include <resources/material/UvScale.h>
#include <api/render/GenericRendererBuilder.h>

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

    Model& ModelInstanceDisplayer::getReferenceModel() const {
        //A reference model is a model which can be used to represent all instance models.
        //For unique properties (e.g. Model#getTransform()#getPosition()): do not use the reference model.
        if (instanceModels.empty()) {
            throw std::runtime_error("No reference model on bared model displayer");
        }
        return *instanceModels[0];
    }

    void ModelInstanceDisplayer::fillMaterialData(const Mesh& mesh) {
        materialData.encodedEmissiveFactor = MathFunction::clamp(mesh.getMaterial().getEmissiveFactor() / Material::MAX_EMISSIVE_FACTOR, 0.0f, 1.0f);
        materialData.ambientFactor = mesh.getMaterial().getAmbientFactor();
    }

    std::vector<Point2<float>> ModelInstanceDisplayer::scaleUv(const std::vector<Point2 < float>>& uvTexture, const UvScale& uvScale) const {
        std::vector<Point2<float>> scaledUvTexture;
        scaledUvTexture.reserve(uvTexture.size());

        const Vector3<float> scale = getReferenceModel().getTransform().getScale();
        for (const Point2<float>& uv : uvTexture) {
            scaledUvTexture.emplace_back(uvScale.scaleU(uv.X, scale), uvScale.scaleV(uv.Y, scale));
        }
        return scaledUvTexture;
    }

    TextureParam ModelInstanceDisplayer::buildTextureParam(const Mesh& mesh) const {
        TextureParam::ReadMode textureReadMode = mesh.getMaterial().repeatTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
        return TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);
    }

    void ModelInstanceDisplayer::notify(Observable* observable, int notificationType) {
        if (const auto* model = dynamic_cast<Model*>(observable)) {
            if (notificationType == Model::MESH_UPDATED) {
                updateMesh(model);
            } else if (notificationType == Model::MATERIAL_UPDATED) {
                updateMaterial(model);
            } else if (notificationType == Model::SCALE_UPDATED) {
                updateScale(model);
            }
        }
    }

    bool ModelInstanceDisplayer::checkUpdateAllowance(const Model* model) const {
        bool canUpdateDisplayer = instanceId == ModelDisplayable::INSTANCING_DENY_ID;
        if (!canUpdateDisplayer) {
            if (model->computeInstanceId(displayMode) == instanceId) {
                //Update without impact on the instance ID, either:
                // - Update has no effect. Examples: change scale from 1.0f to 1.0f, switch between two materials created programmatically and having the exact same properties.
                // - Bug in the way the instance ID is computed
                return true;
            }
        }
        return canUpdateDisplayer;
    }

    void ModelInstanceDisplayer::updateMesh(const Model* model) {
        if (checkUpdateAllowance(model)) {
            unsigned int meshIndex = 0;
            for (const auto& meshRenderer: meshRenderers) {
                const Mesh& mesh = model->getMeshes()->getMesh(meshIndex);
                meshRenderer->updateData(0, mesh.getVertices());
                if (displayMode == DisplayMode::DEFAULT_MODE) {
                    meshRenderer->updateData(2, mesh.getNormals());
                    meshRenderer->updateData(3, mesh.getTangents());
                }

                meshIndex++;
            }
        }
    }

    void ModelInstanceDisplayer::updateMaterial(const Model* model) {
        if (displayMode == DisplayMode::DEFAULT_MODE && checkUpdateAllowance(model)) {
            unsigned int meshIndex = 0;
            for (const auto& meshRenderer: meshRenderers) {
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
    }

    void ModelInstanceDisplayer::updateScale(const Model* model) {
        if (displayMode == DisplayMode::DEFAULT_MODE && checkUpdateAllowance(model)) {
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

    const ModelSetDisplayer& ModelInstanceDisplayer::getModelSetDisplayer() const {
        return modelSetDisplayer;
    }

    std::size_t ModelInstanceDisplayer::getInstanceId() const {
        return instanceId;
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

        model.addObserver(this, Model::MESH_UPDATED);
        model.addObserver(this, Model::MATERIAL_UPDATED);
        model.addObserver(this, Model::SCALE_UPDATED);
    }

    void ModelInstanceDisplayer::removeInstanceModel(Model& model) {
        model.detachModelInstanceDisplayer(*this);
        std::size_t erasedCount = std::erase_if(instanceModels, [&model](const Model* m) {return m == &model;});
        if (erasedCount != 1) {
            throw std::runtime_error("Removing the instance model fail: " + model.getConstMeshes()->getId());
        }

        model.removeObserver(this, Model::MATERIAL_UPDATED);
        model.removeObserver(this, Model::MESH_UPDATED);
        model.removeObserver(this, Model::SCALE_UPDATED);
    }

    unsigned int ModelInstanceDisplayer::getInstanceCount() const {
        return (unsigned int)instanceModels.size();
    }

    void ModelInstanceDisplayer::clearModelForRendering() { //TODO rename
        instanceMatrices.clear();
    }

    void ModelInstanceDisplayer::registerModelForRendering(Model& model) { //TODO check model is in instanceModels ?! or remove instanceModels ?
        InstanceMatrix instanceMatrix;
        instanceMatrix.modelMatrix = model.getTransform().getTransformMatrix();
        instanceMatrix.normalMatrix = instanceMatrix.modelMatrix.inverse().transpose();
        instanceMatrices.push_back(instanceMatrix);
    }

    void ModelInstanceDisplayer::prepareRendering(unsigned int renderingOrder, const Matrix4<float>& projectionViewMatrix, const MeshFilter* meshFilter) const {
        if (instanceMatrices.empty()) {
            return;
        }

        unsigned int meshIndex = 0;
        for (auto& meshRenderer : meshRenderers) {
            const Mesh& mesh = getReferenceModel().getMeshes()->getMesh(meshIndex++);
            if (meshFilter && !meshFilter->isAccepted(mesh)) {
                continue;
            }

            meshRenderer->updateInstanceData(instanceMatrices.size(), (const float*)instanceMatrices.data());
            meshRenderer->updateUniformData(0, &projectionViewMatrix);
            if (customShaderVariable) {
                customShaderVariable->loadCustomShaderVariables(*meshRenderer);
            }

            meshRenderer->enableRenderer(renderingOrder);
        }
    }

    void ModelInstanceDisplayer::drawBBox(GeometryContainer& geometryContainer) {
        for (auto& aabboxModel : aabboxModels) {
            geometryContainer.removeGeometry(*aabboxModel);
        }
        aabboxModels.clear();

        for (Model* instanceModel : instanceModels) {
            aabboxModels.push_back(std::make_shared<AABBoxModel>(instanceModel->getAABBox()));
            geometryContainer.addGeometry(aabboxModels.back());
        }
    }

    void ModelInstanceDisplayer::drawBaseBones(GeometryContainer& geometryContainer, const MeshFilter* meshFilter) const {
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
