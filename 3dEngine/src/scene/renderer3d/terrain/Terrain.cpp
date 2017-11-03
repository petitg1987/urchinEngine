#include <GL/glew.h>
#include <cassert>
#include <stdexcept>

#include "Terrain.h"
#include "utils/shader/ShaderManager.h"

#define DEFAULT_AMBIENT 0.3

namespace urchin
{
    Terrain::Terrain(std::unique_ptr<TerrainMesh> &terrainMesh, std::unique_ptr<TerrainMaterial> &terrainMaterial) :
            terrainMesh(std::move(terrainMesh)),
            terrainMaterial(std::move(terrainMaterial))
    {
        glGenBuffers(5, bufferIDs);
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
        glBufferData(GL_ARRAY_BUFFER, this->terrainMesh->getVertices().size()*sizeof(float)*3, &this->terrainMesh->getVertices()[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
        glVertexAttribPointer(SHADER_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
        glBufferData(GL_ARRAY_BUFFER, this->terrainMesh->getTexCoordinates().size()*sizeof(float)*2, &this->terrainMesh->getTexCoordinates()[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_TEX_COORD);
        glVertexAttribPointer(SHADER_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_NORMAL]);
        glBufferData(GL_ARRAY_BUFFER, this->terrainMesh->getNormals().size()*sizeof(float)*3, &this->terrainMesh->getNormals()[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_NORMAL);
        glVertexAttribPointer(SHADER_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[VAO_INDEX]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->terrainMesh->getIndices().size()*sizeof(unsigned int), &this->terrainMesh->getIndices()[0], GL_STATIC_DRAW);

        shader = ShaderManager::instance()->createProgram("terrain.vert", "terrain.frag");
        ShaderManager::instance()->bind(shader);

        mModelLoc = glGetUniformLocation(shader, "mModel");
        mProjectionLoc = glGetUniformLocation(shader, "mProjection");
        mViewLoc = glGetUniformLocation(shader, "mView");
        ambientLoc = glGetUniformLocation(shader, "ambient");
        setAmbient(DEFAULT_AMBIENT);

        this->terrainMaterial->initialize(shader);
    }

    Terrain::~Terrain()
    {
        glDeleteVertexArrays(1, &vertexArrayObject);
        glDeleteBuffers(5, bufferIDs);

        ShaderManager::instance()->removeProgram(shader);
    }

    void Terrain::onCameraProjectionUpdate(const Matrix4<float> &projectionMatrix)
    {
        this->projectionMatrix = projectionMatrix;
    }

    const std::vector<Point3<float>> &Terrain::getVertices() const
    {
        return terrainMesh->getVertices();
    }

    unsigned int Terrain::getXSize() const
    {
        return terrainMesh->getXSize();
    }

    unsigned int Terrain::getZSize() const
    {
        return terrainMesh->getZSize();
    }

    float Terrain::getAmbient() const
    {
        return ambient;
    }

    void Terrain::setAmbient(float ambient)
    {
        this->ambient = ambient;
    }

    void Terrain::setTransform(const Transform<float> &transform)
    {
        this->transform = transform;
    }

    const Transform<float> &Terrain::getTransform() const
    {
        return transform;
    }

    void Terrain::display(const Matrix4<float> &viewMatrix) const
    {
        unsigned int shaderSaved = ShaderManager::instance()->getCurrentProgram();
        ShaderManager::instance()->bind(shader);

        glUniformMatrix4fv(mModelLoc, 1, GL_FALSE, (const float*) transform.getTransformMatrix());
        glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float*)projectionMatrix);
        glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, (const float*)viewMatrix);
        glUniform1f(ambientLoc, ambient);

        terrainMaterial->loadTextures();

        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(RESTART_INDEX);

        glBindVertexArray(vertexArrayObject);
        glDrawElements(GL_TRIANGLE_STRIP, terrainMesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);

        glDisable(GL_PRIMITIVE_RESTART);

        ShaderManager::instance()->bind(shaderSaved);
    }
}
