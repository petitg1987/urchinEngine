#include <memory>

#include "ModelDisplayer.h"
#include "graphic/render/GenericRendererBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    ModelDisplayer::ModelDisplayer(Model *model, DisplayMode displayMode, std::shared_ptr<RenderTarget> renderTarget) :
            model(model),
            renderTarget(std::move(renderTarget)) {

        for(auto& constMesh : model->getConstMeshes()->getConstMeshes()) {
            auto meshRendererBuilder = std::make_unique<GenericRendererBuilder>(renderTarget, ShapeType::TRIANGLE);
            meshRendererBuilder
                ->enableDepthTest()
                ->addData(&constMesh->getBaseVertices())
                ->addData(&constMesh->getTextureCoordinates())
                ->addData(&constMesh->getBaseNormals())
                ->addData(&constMesh->getBaseTangents())
                ->indices(&constMesh->getTrianglesIndices());

            if(displayMode == DisplayMode::DEFAULT_MODE) {
                TextureParam::ReadMode textureReadMode = constMesh->getMaterial()->isRepeatableTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
                TextureParam textureParam = TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);

                meshRendererBuilder
                    ->addTexture(TextureReader::build(constMesh->getMaterial()->getDiffuseTexture(), textureParam))
                    ->addTexture(TextureReader::build(constMesh->getMaterial()->getNormalTexture(), textureParam));
            }

            meshRenderers.push_back(meshRendererBuilder->build());
        }

        model->addObserver(this, Model::MESH_UPDATED);
    }

    void ModelDisplayer::notify(Observable* observable, int notificationType) {
        if (auto* model = dynamic_cast<Model*>(observable)) {
            assert(this->model == model);
            if (notificationType == Model::MESH_UPDATED) {
                unsigned int meshIndex = 0;
                for(auto& meshRenderer : meshRenderers) {
                    const Mesh* mesh = model->getMeshes()->getMesh(meshIndex);
                    meshRenderer->updateData(0, &mesh->getVertices());
                    meshRenderer->updateData(2, &mesh->getNormals());
                    meshRenderer->updateData(3, &mesh->getTangents());
                }
                meshIndex++;
            }
        }
    }

    void ModelDisplayer::display(const MeshParameter& meshParameter) const {
        unsigned int meshIndex = 0;
        for(auto& meshRenderer : meshRenderers) {
            if (meshParameter.getAmbientFactorShaderVar().isValid()) {
                const ConstMesh* constMesh = model->getConstMeshes()->getConstMesh(meshIndex);
                ShaderDataSender().sendData(meshParameter.getAmbientFactorShaderVar(), constMesh->getMaterial()->getAmbientFactor());
            }

            renderTarget->display(meshRenderer);
            meshIndex++;
        }
    }

}
