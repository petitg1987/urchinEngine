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

            customModelShaderVariable(nullptr),
            depthTestEnabled(true),
            depthWriteEnabled(true) {

    }

    ModelDisplayer::~ModelDisplayer() {
        model->removeObserver(this, Model::MESH_UPDATED);
    }

    void ModelDisplayer::setCustomModelShaderVariable(CustomModelShaderVariable* customModelShaderVariable) {
        if (customModelShaderVariable) {
            if (isInitialized) {
                throw std::runtime_error("Can not define a custom model shader variable on an initialized model displayer: " + model->getConstMeshes()->getName());
            } else if (displayMode == DEFAULT_MODE) {
                //ensure shader binding id are correct
                throw std::runtime_error("Define custom model shader variable in default mode is not implemented: " + model->getConstMeshes()->getName());
            }
        }
        this-> customModelShaderVariable = customModelShaderVariable;
    }

    void ModelDisplayer::setCustomDepthOperations(bool depthTestEnabled, bool depthWriteEnabled) {
        if (isInitialized) {
            throw std::runtime_error("Can not define depth operations on an initialized model displayer: " + model->getConstMeshes()->getName());
        }
        this->depthTestEnabled = depthTestEnabled;
        this->depthWriteEnabled = depthWriteEnabled;
    }

    void ModelDisplayer::setCustomBlendFunctions(const std::vector<BlendFunction>& customBlendFunctions) {
        if (!customBlendFunctions.empty() && isInitialized) {
            throw std::runtime_error("Can not define custom blend functions on an initialized model displayer: " + model->getConstMeshes()->getName());
        }
        this->customBlendFunctions = customBlendFunctions;
    }

    void ModelDisplayer::initialize() {
        if (isInitialized) {
            throw std::runtime_error("Model displayer is already initialized: " + model->getConstMeshes()->getName());
        }

        for (auto& constMesh : model->getConstMeshes()->getConstMeshes()) {
            auto meshName = model->getMeshes()->getConstMeshes().getName();
            auto meshRendererBuilder = GenericRendererBuilder::create("mesh - " + meshName, renderTarget, this->shader, ShapeType::TRIANGLE)
                    ->addData(constMesh->getBaseVertices())
                    ->indices(constMesh->getTrianglesIndices())
                    ->addUniformData(sizeof(projectionMatrix), &projectionMatrix) //binding 0
                    ->addUniformData(sizeof(positioningData), &positioningData); //binding 1

                if (displayMode == DEFAULT_MODE) {
                    meshRendererBuilder->addUniformData(sizeof(meshData), &meshData); //binding 2
                }
                if (customModelShaderVariable) {
                    customModelShaderVariable->setupMeshRenderer(meshRendererBuilder); //binding 2 & 3
                }
                if (depthTestEnabled) {
                    meshRendererBuilder->enableDepthTest();
                }
                if (depthWriteEnabled) {
                    meshRendererBuilder->enableDepthWrite();
                }
                if (!customBlendFunctions.empty()) {
                    meshRendererBuilder->enableTransparency(customBlendFunctions);
                }

                TextureParam::ReadMode textureReadMode = constMesh->getMaterial().isRepeatableTextures() ?
                        TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
                if (displayMode == DEFAULT_MODE || displayMode == DIFFUSE_MODE) {
                    TextureParam diffuseTextureParam = TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);

                    meshRendererBuilder
                            ->addData(constMesh->getTextureCoordinates())
                            ->addUniformTextureReader(TextureReader::build(constMesh->getMaterial().getDiffuseTexture(), std::move(diffuseTextureParam))); //binding 3
                }
                if (displayMode == DEFAULT_MODE) {
                    TextureParam normalTextureParam = TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);
                    meshRendererBuilder
                            ->addData(constMesh->getBaseNormals())
                            ->addData(constMesh->getBaseTangents())
                            ->addUniformTextureReader(TextureReader::build(constMesh->getMaterial().getNormalTexture(), std::move(normalTextureParam))); //binding 4
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

    void ModelDisplayer::prepareRendering(const Matrix4<float>& viewMatrix, const MeshFilter* meshFilter) const {
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
                meshData.ambientFactor = constMesh.getMaterial().getAmbientFactor();
                meshRenderer->updateUniformData(2, &meshData);
            }
            if (customModelShaderVariable) {
                customModelShaderVariable->loadCustomShaderVariables(*meshRenderer);
            }

            meshRenderer->enableRenderer();
        }
    }

    void ModelDisplayer::drawBBox(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) {
        aabboxModel = std::make_unique<AABBoxModel>(model->getAABBox());
        aabboxModel->initialize(renderTarget);
        aabboxModel->onCameraProjectionUpdate(projectionMatrix);
        aabboxModel->prepareRendering(viewMatrix);
    }

    void ModelDisplayer::drawBaseBones(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix, const MeshFilter* meshFilter) const {
        if (model->getMeshes()) {
            for (unsigned int m = 0; m < model->getMeshes()->getNumberMeshes(); ++m) {
                const ConstMesh& constMesh = model->getMeshes()->getConstMeshes().getConstMesh(m);
                if (!meshFilter || meshFilter->isAccepted(constMesh)) {
                    Matrix4<float> modelViewMatrix = viewMatrix * model->getTransform().getTransformMatrix();
                    model->getMeshes()->getMesh(m).drawBaseBones(renderTarget, projectionMatrix, modelViewMatrix);
                }
            }
        }
    }

}
