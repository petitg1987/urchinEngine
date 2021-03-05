#include "Mesh.h"
#include "resources/model/MeshService.h"
#include "resources/geometry/points/PointsModel.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    Mesh::Mesh(const ConstMesh* constMesh) :
            constMesh(constMesh) {

        TextureParam::ReadMode textureReadMode = constMesh->getMaterial()->isRepeatableTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
        TextureParam textureParam = TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);

        meshRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::TRIANGLE)
                ->enableDepthTest()
                ->addData(&constMesh->getBaseVertices())
                ->addData(&constMesh->getTextureCoordinates())
                ->addData(&constMesh->getBaseNormals())
                ->addData(&constMesh->getBaseTangents())
                ->indices(&constMesh->getTrianglesIndices())
                ->addTexture(TextureReader::build(constMesh->getMaterial()->getDiffuseTexture(), textureParam))
                ->addTexture(TextureReader::build(constMesh->getMaterial()->getNormalTexture(), textureParam))
                ->build();
    }

    void Mesh::update(const std::vector<Bone>& skeleton) {
        //recompute the vertices and normals
        MeshService::computeVertices(constMesh, skeleton, vertices);
        MeshService::computeNormalsAndTangents(constMesh, vertices, normals, tangents);

        meshRenderer->updateData(0, &vertices);
        meshRenderer->updateData(2, &normals);
        meshRenderer->updateData(3, &tangents);
    }

    void Mesh::display(const RenderTarget* renderTarget, const MeshParameter& meshParameter) const {
        if (meshParameter.getAmbientFactorShaderVar().isValid()) {
            ShaderDataSender().sendData(meshParameter.getAmbientFactorShaderVar(), constMesh->getMaterial()->getAmbientFactor());
        }

        renderTarget->display(meshRenderer);
    }

    void Mesh::drawBaseBones(const std::shared_ptr<RenderTarget>& renderTarget, const Matrix4<float>& projectionMatrix, const Matrix4<float>& viewMatrix) const {
        std::vector<Point3<float>> bonePositions;
        for (const auto& bone : constMesh->getBaseSkeleton()) {
            bonePositions.push_back(bone.pos);
        }

        std::unique_ptr<PointsModel> pointsModel = std::make_unique<PointsModel>(bonePositions);
        pointsModel->initialize(renderTarget);
        pointsModel->setOutlineSize(5.0f);
        pointsModel->onCameraProjectionUpdate(projectionMatrix);
        pointsModel->setAlwaysVisible(true);
        pointsModel->display(viewMatrix);
    }

}
