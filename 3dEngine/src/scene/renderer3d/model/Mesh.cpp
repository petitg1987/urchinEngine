#include "Mesh.h"
#include "resources/model/MeshService.h"
#include "resources/geometry/points/PointsModel.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    Mesh::Mesh(const ConstMesh *constMesh) :
            constMesh(constMesh) {

        unsigned int diffuseTextureId = constMesh->getMaterial()->getDiffuseTexture()->getTextureID();
        unsigned int normalTextureId = constMesh->getMaterial()->getNormalTexture()->getTextureID();
        TextureParam::ReadMode textureReadMode = constMesh->getMaterial()->isRepeatableTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
        TextureParam textureParam = TextureParam::build(textureReadMode, TextureParam::LINEAR_MIPMAP, TextureParam::ANISOTROPY);

        meshRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::TRIANGLE)
                ->enableDepthTest()
                ->addData(&constMesh->getBaseVertices())
                ->addData(&constMesh->getTextureCoordinates())
                ->addData(&constMesh->getBaseNormals())
                ->addData(&constMesh->getBaseTangents())
                ->indices(&constMesh->getTrianglesIndices())
                ->addTexture(Texture::build(diffuseTextureId, Texture::DEFAULT, textureParam))
                ->addTexture(Texture::build(normalTextureId, Texture::DEFAULT, textureParam))
                ->build();
    }

    void Mesh::update(const std::vector<Bone> &skeleton) {
        //recompute the vertices and normals
        MeshService::instance()->computeVertices(constMesh, skeleton, vertices);
        MeshService::instance()->computeNormalsAndTangents(constMesh, vertices, normals, tangents);

        meshRenderer->updateData(0, &vertices);
        meshRenderer->updateData(2, &normals);
        meshRenderer->updateData(3, &tangents);
    }

    void Mesh::display(const MeshParameter &meshParameter) const {
        if (meshParameter.getAmbientFactorShaderVar().isValid()) {
            ShaderDataSender().sendData(meshParameter.getAmbientFactorShaderVar(), constMesh->getMaterial()->getAmbientFactor());
        }

        meshRenderer->renderTextures(meshParameter.needRenderTextures());
        meshRenderer->draw();
    }

    void Mesh::drawBaseBones(const Matrix4<float> &projectionMatrix, const Matrix4<float> &viewMatrix) const {
        std::vector<Point3<float>> bonePositions;
        for (const auto &bone : constMesh->getBaseSkeleton()) {
            bonePositions.push_back(bone.pos);
        }

        std::unique_ptr<PointsModel> pointsModel = std::make_unique<PointsModel>(bonePositions);
        pointsModel->setOutlineSize(5.0f);
        pointsModel->onCameraProjectionUpdate(projectionMatrix);
        pointsModel->setAlwaysVisible(true);
        pointsModel->display(viewMatrix);
    }

}
