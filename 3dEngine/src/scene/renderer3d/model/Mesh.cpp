#include <GL/glew.h>

#include "Mesh.h"
#include "resources/model/MeshService.h"
#include "resources/geometry/points/PointsModel.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    Mesh::Mesh(const ConstMesh *constMesh) :
        constMesh(constMesh),
        bufferIDs(),
        vertexArrayObject(0) {
        //visual
        glGenBuffers(5, bufferIDs);
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
        glBufferData(GL_ARRAY_BUFFER, constMesh->getNumberVertices()*sizeof(float)*3, &constMesh->getBaseVertices()[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
        glVertexAttribPointer(SHADER_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
        glBufferData(GL_ARRAY_BUFFER, constMesh->getNumberVertices()*sizeof(float)*2, &constMesh->getTextureCoordinates()[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_TEX_COORD);
        glVertexAttribPointer(SHADER_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_NORMAL]);
        glBufferData(GL_ARRAY_BUFFER, constMesh->getNumberVertices()*sizeof(float)*3, &constMesh->getBaseNormals()[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_NORMAL);
        glVertexAttribPointer(SHADER_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TANGENT]);
        glBufferData(GL_ARRAY_BUFFER, constMesh->getNumberVertices()*sizeof(float)*3, &constMesh->getBaseTangents()[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_TANGENT);
        glVertexAttribPointer(SHADER_TANGENT, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[VAO_INDEX]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, constMesh->getNumberTriangles()*3*sizeof(unsigned int), &constMesh->getTriangles()[0], GL_STATIC_DRAW); //TODO use std::vector<unsigned int> ?
    }

    Mesh::~Mesh() {
        glDeleteVertexArrays(1, &vertexArrayObject);
        glDeleteBuffers(5, bufferIDs);
    }

    void Mesh::update(const std::vector<Bone> &skeleton) {
        //recompute the vertices and normals
        MeshService::instance()->computeVertices(constMesh, skeleton, vertices);
        MeshService::instance()->computeNormalsAndTangents(constMesh, vertices, normals, tangents);

        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
        glBufferData(GL_ARRAY_BUFFER, constMesh->getNumberVertices()*sizeof(float)*3, &vertices[0], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_NORMAL]);
        glBufferData(GL_ARRAY_BUFFER, constMesh->getNumberVertices()*sizeof(float)*3, &normals[0], GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TANGENT]);
        glBufferData(GL_ARRAY_BUFFER, constMesh->getNumberVertices()*sizeof(float)*3, &tangents[0], GL_DYNAMIC_DRAW);
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

        glBindVertexArray(vertexArrayObject);
        glDrawElements(GL_TRIANGLES, constMesh->getNumberTriangles()*3, GL_UNSIGNED_INT, nullptr);
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
