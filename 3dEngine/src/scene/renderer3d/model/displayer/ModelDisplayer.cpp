#include <memory>

#include "ModelDisplayer.h"
#include "graphic/render/GenericRendererBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "resources/geometry/aabbox/AABBoxModel.h"

namespace urchin {

    ModelDisplayer::ModelDisplayer(Model *model, DisplayMode displayMode, std::shared_ptr<RenderTarget> renderTarget, std::shared_ptr<Shader> shader) :
            model(model),
            displayMode(displayMode),
            renderTarget(std::move(renderTarget)),
            shader(std::move(shader)) {

        for (auto& constMesh : model->getConstMeshes()->getConstMeshes()) {
            auto meshRendererBuilder = std::make_unique<GenericRendererBuilder>(this->renderTarget, this->shader, ShapeType::TRIANGLE);
            meshRendererBuilder
                ->enableDepthTest()
                ->addData(&constMesh->getBaseVertices())
                ->addData(&constMesh->getTextureCoordinates())
                ->addData(&constMesh->getBaseNormals())
                ->addData(&constMesh->getBaseTangents())
                ->indices(&constMesh->getTrianglesIndices());

            if (displayMode == DEFAULT_MODE) {
                TextureParam::ReadMode textureReadMode = constMesh->getMaterial()->isRepeatableTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
                TextureParam textureParam = TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);

                meshRendererBuilder
                    ->addTextureReader(TextureReader::build(constMesh->getMaterial()->getDiffuseTexture(), textureParam))
                    ->addTextureReader(TextureReader::build(constMesh->getMaterial()->getNormalTexture(), textureParam));
            }

            meshRenderers.push_back(meshRendererBuilder->build());
        }

        mModelShaderVar = ShaderVar(this->shader, "mModel");
        if (displayMode == DEFAULT_MODE) {
            mNormalShaderVar = ShaderVar(this->shader, "mNormal");
        } else if (displayMode == DEPTH_ONLY_MODE) {
            mNormalShaderVar = ShaderVar();
        } else {
            throw std::invalid_argument("Unknown display mode: " + std::to_string(displayMode));
        }

        model->addObserver(this, Model::MESH_UPDATED);
    }

    ModelDisplayer::~ModelDisplayer() {
        model->removeObserver(this, Model::MESH_UPDATED);
    }

    void ModelDisplayer::notify(Observable* observable, int notificationType) {
        if (auto* model = dynamic_cast<Model*>(observable)) {
            if (notificationType == Model::MESH_UPDATED) {
                unsigned int meshIndex = 0;
                for (auto& meshRenderer : meshRenderers) {
                    const Mesh* mesh = model->getMeshes()->getMesh(meshIndex);
                    meshRenderer->updateData(0, &mesh->getVertices());
                    meshRenderer->updateData(2, &mesh->getNormals());
                    meshRenderer->updateData(3, &mesh->getTangents());

                    meshIndex++;
                }
            }
        }
    }

    void ModelDisplayer::display(const MeshParameter& meshParameter) const {
        unsigned int meshIndex = 0;
        for (auto& meshRenderer : meshRenderers) {
            if (meshParameter.getAmbientFactorShaderVar().isValid()) {
                const ConstMesh* constMesh = model->getConstMeshes()->getConstMesh(meshIndex);
                ShaderDataSender().sendData(meshParameter.getAmbientFactorShaderVar(), constMesh->getMaterial()->getAmbientFactor());
            }
            if (displayMode == DEFAULT_MODE) {
                ShaderDataSender().sendData(mNormalShaderVar, model->getTransform().getTransformMatrix().toMatrix3().inverse().transpose());
            }
            ShaderDataSender().sendData(mModelShaderVar, model->getTransform().getTransformMatrix());

            renderTarget->display(meshRenderer);
            meshIndex++;
        }
    }

    void ModelDisplayer::drawBBox(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) const {
        AABBoxModel aabboxModel(model->getAABBox());
        aabboxModel.initialize(renderTarget);
        aabboxModel.onCameraProjectionUpdate(projectionMatrix);
        aabboxModel.display(viewMatrix);
    }

    void ModelDisplayer::drawBaseBones(const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) const {
        if (model->getMeshes()) {
            for (unsigned int m = 0; m < model->getMeshes()->getNumberMeshes(); ++m) {
                Matrix4<float> modelViewMatrix = viewMatrix * model->getTransform().getTransformMatrix();
                model->getMeshes()->getMesh(m)->drawBaseBones(renderTarget, projectionMatrix, modelViewMatrix);
            }
        }
    }

}
