#include <memory>

#include <scene/renderer3d/model/displayer/ModelDisplayer.h>
#include <graphic/render/GenericRendererBuilder.h>

namespace urchin {

    ModelDisplayer::ModelDisplayer(Model *model, const Matrix4<float>& projectionMatrix, DisplayMode displayMode, RenderTarget& renderTarget,
                                   const Shader& shader, CustomModelShaderVariable* customModelShaderVariable) :
            model(model),
            displayMode(displayMode),
            renderTarget(renderTarget),
            shader(shader),
            customModelShaderVariable(customModelShaderVariable) {

        for (auto& constMesh : model->getConstMeshes()->getConstMeshes()) {
            auto meshName = model->getMeshes()->getConstMeshes()->getName();
            auto meshRendererBuilder = GenericRendererBuilder::create("mesh - " + meshName, renderTarget, this->shader, ShapeType::TRIANGLE)
                ->enableDepthOperations()
                ->addData(constMesh->getBaseVertices())
                ->indices(constMesh->getTrianglesIndices())
                ->addUniformData(sizeof(projectionMatrix), &projectionMatrix) //binding 0
                ->addUniformData(sizeof(meshData), &meshData); //binding 1
            if (customModelShaderVariable) {
                customModelShaderVariable->setupMeshRenderer(meshRendererBuilder); //binding 2 & 3
            }

            if (displayMode == DEFAULT_MODE) {
                assert(!customModelShaderVariable); //ensure binding id are correct
                TextureParam::ReadMode textureReadMode = constMesh->getMaterial()->isRepeatableTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
                TextureParam textureParam = TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);

                meshRendererBuilder
                    ->addData(constMesh->getTextureCoordinates())
                    ->addData(constMesh->getBaseNormals())
                    ->addData(constMesh->getBaseTangents())
                    ->addUniformTextureReader(TextureReader::build(constMesh->getMaterial()->getDiffuseTexture(), textureParam)) //binding 2
                    ->addUniformTextureReader(TextureReader::build(constMesh->getMaterial()->getNormalTexture(), textureParam)); //binding 3
            }

            meshRenderers.push_back(meshRendererBuilder->build());
        }

        model->addObserver(this, Model::MESH_UPDATED);
    }

    ModelDisplayer::~ModelDisplayer() {
        model->removeObserver(this, Model::MESH_UPDATED);
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

    void ModelDisplayer::prepareRendering(const Matrix4<float>& viewMatrix) const {
        unsigned int meshIndex = 0;
        for (auto& meshRenderer : meshRenderers) {
            meshData.viewMatrix = viewMatrix;
            meshData.modelMatrix = model->getTransform().getTransformMatrix();
            meshData.normalMatrix = (displayMode == DEFAULT_MODE) ? model->getTransform().getTransformMatrix().inverse().transpose() : Matrix4<float>();
            meshData.ambientFactor = model->getConstMeshes()->getConstMesh(meshIndex).getMaterial()->getAmbientFactor();

            meshRenderer->updateUniformData(1, &meshData);

            if (customModelShaderVariable) {
                customModelShaderVariable->loadCustomShaderVariables(*meshRenderer);
            }

            meshRenderer->enableRenderer();
            meshIndex++;
        }
    }

    void ModelDisplayer::drawBBox(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) {
        aabboxModel = std::make_unique<AABBoxModel>(model->getAABBox());
        aabboxModel->initialize(renderTarget);
        aabboxModel->onCameraProjectionUpdate(projectionMatrix);
        aabboxModel->prepareRendering(viewMatrix);
    }

    void ModelDisplayer::drawBaseBones(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) const {
        if (model->getMeshes()) {
            for (unsigned int m = 0; m < model->getMeshes()->getNumberMeshes(); ++m) {
                Matrix4<float> modelViewMatrix = viewMatrix * model->getTransform().getTransformMatrix();
                model->getMeshes()->getMesh(m).drawBaseBones(renderTarget, projectionMatrix, modelViewMatrix);
            }
        }
    }

}
