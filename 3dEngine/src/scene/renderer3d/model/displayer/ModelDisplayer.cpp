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
            meshData({}),
            customShaderVariable(nullptr),
            depthTestEnabled(true),
            depthWriteEnabled(true),
            enableFaceCull(true) {

    }

    ModelDisplayer::~ModelDisplayer() {
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

        for (auto& constMesh : model->getConstMeshes()->getConstMeshes()) {
            auto meshName = model->getMeshes()->getConstMeshes().getMeshesName();
            auto meshRendererBuilder = GenericRendererBuilder::create("mesh - " + meshName, renderTarget, this->shader, ShapeType::TRIANGLE)
                    ->addData(constMesh->getBaseVertices())
                    ->indices(constMesh->getTrianglesIndices())
                    ->addUniformData(sizeof(projectionMatrix), &projectionMatrix) //binding 0
                    ->addUniformData(sizeof(positioningData), &positioningData) //binding 1
                    ->addUniformData(sizeof(meshData), &meshData); //binding 2 (only useful/updated for DEFAULT_MODE)

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

                auto textureReadMode = constMesh->getMaterial().isRepeatTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
                if (displayMode == DEFAULT_MODE) {
                    TextureParam diffuseTextureParam = TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);
                    TextureParam normalTextureParam = TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);

                    meshRendererBuilder
                            ->addData(constMesh->getTextureCoordinates())
                            ->addData(constMesh->getBaseNormals())
                            ->addData(constMesh->getBaseTangents())
                            ->addUniformTextureReader(TextureReader::build(constMesh->getMaterial().getDiffuseTexture(), std::move(diffuseTextureParam))) //binding 5
                            ->addUniformTextureReader(TextureReader::build(constMesh->getMaterial().getNormalTexture(), std::move(normalTextureParam))); //binding 6
                }

                meshRenderers.push_back(meshRendererBuilder->build());
        }

        model->addObserver(this, Model::MESH_UPDATED);

        isInitialized = true;
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
            }
        }
    }

    void ModelDisplayer::prepareRendering(unsigned int& renderingOrder, const Matrix4<float>& viewMatrix, const MeshFilter* meshFilter) const {
        unsigned int meshIndex = 0;
        for (auto& meshRenderer : meshRenderers) {
            const ConstMesh& constMesh = model->getConstMeshes()->getConstMesh(meshIndex++);
            if (meshFilter && !meshFilter->isAccepted(constMesh)) {
                continue;
            }

            positioningData.viewMatrix = viewMatrix;
            positioningData.modelMatrix = model->getTransform().getTransformMatrix();
            meshRenderer->updateUniformData(1, &positioningData);

            if (displayMode == DEFAULT_MODE) {
                meshData.normalMatrix = model->getTransform().getTransformMatrix().inverse().transpose();
                meshData.encodedEmissiveFactor = MathFunction::clamp(constMesh.getMaterial().getEmissiveFactor() / Material::MAX_EMISSIVE_FACTOR, 0.0f, 1.0f);
                meshData.ambientFactor = constMesh.getMaterial().getAmbientFactor();
                meshRenderer->updateUniformData(2, &meshData);
            }
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
                const ConstMesh& constMesh = model->getMeshes()->getConstMeshes().getConstMesh(m);
                if (!meshFilter || meshFilter->isAccepted(constMesh)) {
                    model->getMeshes()->getMesh(m).drawBaseBones(geometryContainer, model->getTransform().getTransformMatrix());
                }
            }
        }
    }

}
