#include <memory>

#include <scene/renderer3d/model/displayer/ModelDisplayer.h>
#include <graphic/render/GenericRendererBuilder.h>

namespace urchin {

    ModelDisplayer::ModelDisplayer(Model *model, const Matrix4<float>& projectionMatrix, DisplayMode displayMode, RenderTarget& renderTarget, const Shader& shader) :
            isInitialized(false),
            model(model),
            projectionMatrix(projectionMatrix),
            displayMode(displayMode),
            renderTarget(renderTarget),
            shader(shader),
            positioningData({}),
            materialData({}),
            customShaderVariable(nullptr),
            depthTestEnabled(true),
            depthWriteEnabled(true),
            enableFaceCull(true) {

    }

    ModelDisplayer::~ModelDisplayer() {
        model->removeObserver(this, Model::MATERIAL_UPDATED);
        model->removeObserver(this, Model::MESH_UPDATED);
    }

    void ModelDisplayer::setupCustomShaderVariable(CustomModelShaderVariable* customShaderVariable) {
        if (customShaderVariable && isInitialized) {
            throw std::runtime_error("Can not define a custom model shader variable on an initialized model displayer: " + model->getConstMeshes()->getName());
        }
        this->customShaderVariable = customShaderVariable;
    }

    void ModelDisplayer::setupDepthOperations(bool depthTestEnabled, bool depthWriteEnabled) {
        if (isInitialized) {
            throw std::runtime_error("Can not define depth operations on an initialized model displayer: " + model->getConstMeshes()->getName());
        }
        this->depthTestEnabled = depthTestEnabled;
        this->depthWriteEnabled = depthWriteEnabled;
    }

    void ModelDisplayer::setupFaceCull(bool enableFaceCull) {
        if (isInitialized) {
            throw std::runtime_error("Can not define face cull flag on an initialized model displayer: " + model->getConstMeshes()->getName());
        }
        this->enableFaceCull = enableFaceCull;
    }

    void ModelDisplayer::setupBlendFunctions(const std::vector<BlendFunction>& blendFunctions) {
        if (!blendFunctions.empty() && isInitialized) {
            throw std::runtime_error("Can not define blend functions on an initialized model displayer: " + model->getConstMeshes()->getName());
        }
        this->blendFunctions = blendFunctions;
    }

    void ModelDisplayer::initialize() {
        if (isInitialized) {
            throw std::runtime_error("Model displayer is already initialized: " + model->getConstMeshes()->getName());
        }

        for (unsigned int i = 0; i < model->getMeshes()->getNumberMeshes(); ++i) {
            const ConstMesh& constMesh = model->getConstMeshes()->getConstMesh(i);
            const Mesh& mesh = model->getMeshes()->getMesh(i);
            auto meshName = model->getMeshes()->getConstMeshes().getMeshesName();

            fillMaterialData(mesh);

            auto meshRendererBuilder = GenericRendererBuilder::create("mesh - " + meshName, renderTarget, this->shader, ShapeType::TRIANGLE)
                    ->addData(mesh.getVertices())
                    ->indices(constMesh.getTrianglesIndices())
                    ->addUniformData(sizeof(projectionMatrix), &projectionMatrix) //binding 0
                    ->addUniformData(sizeof(positioningData), &positioningData) //binding 1
                    ->addUniformData(sizeof(materialData), &materialData); //binding 2 (only used in DEFAULT_MODE)

                if (customShaderVariable) {
                    customShaderVariable->setupMeshRenderer(meshRendererBuilder); //binding 3 & 4 (optional)
                }
                int missingUniformData = 5 - (int)meshRendererBuilder->getUniformData().size();
                assert(missingUniformData >= 0);
                for (int i = 0; i < missingUniformData; ++i) {
                    int customDummyValue = 0;
                    meshRendererBuilder->addUniformData(sizeof(customDummyValue), &customDummyValue); //binding 3 & 4
                }

                if (depthTestEnabled) {
                    meshRendererBuilder->enableDepthTest();
                }
                if (depthWriteEnabled) {
                    meshRendererBuilder->enableDepthWrite();
                }
                if (!enableFaceCull) {
                    meshRendererBuilder->disableCullFace();
                }
                if (!blendFunctions.empty()) {
                    meshRendererBuilder->enableTransparency(blendFunctions);
                }

                if (displayMode == DEFAULT_MODE) {
                    meshRendererBuilder
                            ->addData(constMesh.getTextureCoordinates())
                            ->addData(mesh.getNormals())
                            ->addData(mesh.getTangents())
                            ->addUniformTextureReader(TextureReader::build(mesh.getMaterial().getDiffuseTexture(), buildTextureParam(mesh))) //binding 5
                            ->addUniformTextureReader(TextureReader::build(mesh.getMaterial().getNormalTexture(), buildTextureParam(mesh))); //binding 6
                }

                meshRenderers.push_back(meshRendererBuilder->build());
        }

        model->addObserver(this, Model::MESH_UPDATED);
        model->addObserver(this, Model::MATERIAL_UPDATED);

        isInitialized = true;
    }

    void ModelDisplayer::fillMaterialData(const Mesh& mesh) {
        materialData.encodedEmissiveFactor = MathFunction::clamp(mesh.getMaterial().getEmissiveFactor() / Material::MAX_EMISSIVE_FACTOR, 0.0f, 1.0f);
        materialData.ambientFactor = mesh.getMaterial().getAmbientFactor();
    }

    TextureParam ModelDisplayer::buildTextureParam(const Mesh& mesh) const {
        auto textureReadMode = mesh.getMaterial().isRepeatTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
        return TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);
    }

    void ModelDisplayer::onCameraProjectionUpdate(const Camera& camera) {
        for (auto& meshRenderer : meshRenderers) {
            meshRenderer->updateUniformData(0, &camera.getProjectionMatrix());
        }
    }

    void ModelDisplayer::notify(Observable* observable, int notificationType) {
        if (auto* model = dynamic_cast<Model*>(observable)) {
            if (notificationType == Model::MESH_UPDATED) {
                unsigned int meshIndex = 0;
                for (auto& meshRenderer : meshRenderers) {
                    const Mesh& mesh = model->getMeshes()->getMesh(meshIndex);
                    meshRenderer->updateData(0, mesh.getVertices());
                    if (displayMode == DEFAULT_MODE) {
                        meshRenderer->updateData(2, mesh.getNormals());
                        meshRenderer->updateData(3, mesh.getTangents());
                    }

                    meshIndex++;
                }
            } else if (notificationType == Model::MATERIAL_UPDATED) {
                if (displayMode == DEFAULT_MODE) {
                    unsigned int meshIndex = 0;
                    for (auto& meshRenderer : meshRenderers) {
                        const Mesh& mesh = model->getMeshes()->getMesh(meshIndex);

                        fillMaterialData(mesh);
                        meshRenderer->updateUniformData(2, &materialData);

                        if(meshRenderer->getUniformTextureReader(5)->getTexture() != mesh.getMaterial().getDiffuseTexture().get()) {
                            meshRenderer->updateUniformTextureReader(5, TextureReader::build(mesh.getMaterial().getDiffuseTexture(), buildTextureParam(mesh)));
                        }
                        if(meshRenderer->getUniformTextureReader(6)->getTexture() != mesh.getMaterial().getNormalTexture().get()) {
                            meshRenderer->updateUniformTextureReader(6, TextureReader::build(mesh.getMaterial().getNormalTexture(), buildTextureParam(mesh)));
                        }

                        meshIndex++;
                    }
                }
            }
        }
    }

    void ModelDisplayer::prepareRendering(unsigned int& renderingOrder, const Matrix4<float>& viewMatrix, const MeshFilter* meshFilter) const {
        unsigned int meshIndex = 0;
        for (auto& meshRenderer : meshRenderers) {
            const Mesh& mesh = model->getMeshes()->getMesh(meshIndex++);
            if (meshFilter && !meshFilter->isAccepted(mesh)) {
                continue;
            }

            positioningData.viewMatrix = viewMatrix;
            positioningData.modelMatrix = model->getTransform().getTransformMatrix();
            positioningData.normalMatrix = model->getTransform().getTransformMatrix().inverse().transpose();
            meshRenderer->updateUniformData(1, &positioningData);

            if (customShaderVariable) {
                customShaderVariable->loadCustomShaderVariables(*meshRenderer);
            }

            meshRenderer->enableRenderer(renderingOrder);
        }
    }

    void ModelDisplayer::drawBBox(GeometryContainer& geometryContainer) {
        if (aabboxModel) {
            geometryContainer.removeGeometry(*aabboxModel);
        }

        aabboxModel = std::make_shared<AABBoxModel>(model->getAABBox());
        geometryContainer.addGeometry(aabboxModel);
    }

    void ModelDisplayer::drawBaseBones(GeometryContainer& geometryContainer, const MeshFilter* meshFilter) const {
        if (model->getMeshes()) {
            for (unsigned int m = 0; m < model->getMeshes()->getNumberMeshes(); ++m) {
                const Mesh& mesh = model->getMeshes()->getMesh(m);
                if (!meshFilter || meshFilter->isAccepted(mesh)) {
                    model->getMeshes()->getMesh(m).drawBaseBones(geometryContainer, model->getTransform().getTransformMatrix());
                }
            }
        }
    }

}
