#include <GL/glew.h>

#include "Mesh.h"
#include "resources/model/MeshService.h"
#include "resources/geometry/points/PointsModel.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    Mesh::Mesh(const ConstMesh *constMesh) :
            constMesh(constMesh) {
        meshRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::TRIANGLE)
                ->addData(&constMesh->getBaseVertices())
                ->addData(&constMesh->getTextureCoordinates())
                ->addData(&constMesh->getBaseNormals())
                ->addData(&constMesh->getBaseTangents())
                ->indices(&constMesh->getTrianglesIndices())
                ->enableDepthTest()
                //->addTexture(Texture::build(texInfoDefault->getTextureID())) //TODO add texture
                ->build();
    }

    void Mesh::update(const std::vector<Bone> &skeleton) {
        //recompute the vertices and normals
        MeshService::instance()->computeVertices(constMesh, skeleton, vertices);
        MeshService::instance()->computeNormalsAndTangents(constMesh, vertices, normals, tangents);

        meshRenderer->updateData(0, &vertices);
        meshRenderer->updateData(2, &normals); //TODO add animate model to test
        meshRenderer->updateData(3, &tangents);
    }

    void Mesh::display(const MeshParameter &meshParameter) const {
        if (meshParameter.getDiffuseTextureUnit() != -1) {
            glActiveTexture(GL_TEXTURE0 + meshParameter.getDiffuseTextureUnit());
            glBindTexture(GL_TEXTURE_2D, constMesh->getMaterial()->getDiffuseTexture()->getTextureID());
        }

        if (meshParameter.getNormalTextureUnit() != -1) {
            glActiveTexture(GL_TEXTURE0 + meshParameter.getNormalTextureUnit());
            glBindTexture(GL_TEXTURE_2D, constMesh->getMaterial()->getNormalTexture()->getTextureID());
        }

        if (meshParameter.getAmbientFactorShaderVar().isValid()) {
            ShaderDataSender().sendData(meshParameter.getAmbientFactorShaderVar(), constMesh->getMaterial()->getAmbientFactor());
        }

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
