#include <GL/glew.h>
#include <cassert>
#include <stdexcept>

#include "Terrain.h"
#include "utils/shader/ShaderManager.h"

#define DEFAULT_AMBIENT 0.3

namespace urchin
{
    Terrain::Terrain(std::unique_ptr<TerrainMesh> &terrainMesh, std::unique_ptr<TerrainMaterial> &terrainMaterial)
    {
        glGenBuffers(5, bufferIDs);
        glGenVertexArrays(1, &vertexArrayObject);

        terrainShader = ShaderManager::instance()->createProgram("terrain.vert", "terrain.frag");
        ShaderManager::instance()->bind(terrainShader);

        mModelLoc = glGetUniformLocation(terrainShader, "mModel");
        mProjectionLoc = glGetUniformLocation(terrainShader, "mProjection");
        mViewLoc = glGetUniformLocation(terrainShader, "mView");
        ambientLoc = glGetUniformLocation(terrainShader, "ambient");
        setAmbient(DEFAULT_AMBIENT);

        setupTerrain(terrainMesh, terrainMaterial);
    }

    Terrain::~Terrain()
    {
        glDeleteVertexArrays(1, &vertexArrayObject);
        glDeleteBuffers(5, bufferIDs);

        ShaderManager::instance()->removeProgram(terrainShader);
    }

    void Terrain::setupTerrain(std::unique_ptr<TerrainMesh> &terrainMesh, std::unique_ptr<TerrainMaterial> &terrainMaterial)
    {
        mesh = std::move(terrainMesh);
        glBindVertexArray(vertexArrayObject);
        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
        glBufferData(GL_ARRAY_BUFFER, mesh->getVertices().size()*sizeof(float)*3, &mesh->getVertices()[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
        glVertexAttribPointer(SHADER_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_NORMAL]);
        glBufferData(GL_ARRAY_BUFFER, mesh->getNormals().size()*sizeof(float)*3, &mesh->getNormals()[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_NORMAL);
        glVertexAttribPointer(SHADER_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[VAO_INDEX]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->getIndices().size()*sizeof(unsigned int), &mesh->getIndices()[0], GL_STATIC_DRAW);

        material = std::move(terrainMaterial);
        material->initialize(terrainShader, mesh->getXSize(), mesh->getZSize());
        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
        glBufferData(GL_ARRAY_BUFFER, material->getTexCoordinates().size()*sizeof(float)*2, &material->getTexCoordinates()[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_TEX_COORD);
        glVertexAttribPointer(SHADER_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    void Terrain::onCameraProjectionUpdate(const Matrix4<float> &projectionMatrix)
    {
        this->projectionMatrix = projectionMatrix;

        ShaderManager::instance()->bind(terrainShader);
        glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float*)projectionMatrix);
    }

    const std::vector<Point3<float>> &Terrain::getVertices() const
    {
        return mesh->getVertices();
    }

    unsigned int Terrain::getXSize() const
    {
        return mesh->getXSize();
    }

    unsigned int Terrain::getZSize() const
    {
        return mesh->getZSize();
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
        ShaderManager::instance()->bind(terrainShader);

        glUniformMatrix4fv(mModelLoc, 1, GL_FALSE, (const float*) transform.getTransformMatrix());
        glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, (const float*)viewMatrix);
        glUniform1f(ambientLoc, ambient);

        material->loadTextures();

        glBindVertexArray(vertexArrayObject);
        glDrawElements(GL_TRIANGLE_STRIP, mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);
    }
}
