#include <GL/glew.h>
#include <cassert>
#include <stdexcept>

#include "Terrain.h"
#include "utils/shader/ShaderManager.h"

#define DEFAULT_AMBIENT 0.3

namespace urchin
{
    Terrain::Terrain(std::unique_ptr<TerrainMesh> &mesh, std::unique_ptr<TerrainMaterial> &material)
    {
        glGenBuffers(5, bufferIDs);
        glGenVertexArrays(1, &vertexArrayObject);

        terrainShader = ShaderManager::instance()->createProgram("terrain.vert", "terrain.frag");
        ShaderManager::instance()->bind(terrainShader);

        vPositionLoc = glGetUniformLocation(terrainShader, "vPosition");
        mProjectionLoc = glGetUniformLocation(terrainShader, "mProjection");
        mViewLoc = glGetUniformLocation(terrainShader, "mView");
        ambientLoc = glGetUniformLocation(terrainShader, "ambient");

        setPosition(Point3<float>(0.0f, 0.0f, 0.0f));
        setAmbient(DEFAULT_AMBIENT);
        setMesh(mesh);
        setMaterial(material);
    }

    Terrain::~Terrain()
    {
        glDeleteVertexArrays(1, &vertexArrayObject);
        glDeleteBuffers(5, bufferIDs);

        ShaderManager::instance()->removeProgram(terrainShader);
    }

    void Terrain::onCameraProjectionUpdate(const Matrix4<float> &projectionMatrix)
    {
        this->projectionMatrix = projectionMatrix;

        ShaderManager::instance()->bind(terrainShader);
        glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float*)projectionMatrix);

        if(grass!=nullptr)
        {
            grass->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void Terrain::setMesh(std::unique_ptr<TerrainMesh> &terrainMesh)
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

        if(material != nullptr)
        {
            setMaterial(material); //material uses mesh info: refresh is required
            //TODO do it for grass
        }
    }

    const TerrainMesh *Terrain::getMesh() const
    {
        return mesh.get();
    }

    void Terrain::setMaterial(std::unique_ptr<TerrainMaterial> &terrainMaterial)
    {
        if(material != terrainMaterial)
        {
            material = std::move(terrainMaterial);
            material->initialize(terrainShader, mesh->getXSize(), mesh->getZSize()); //TODO should be re-executed in case of new mesh provided !
        }

        glBindVertexArray(vertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
        glBufferData(GL_ARRAY_BUFFER, material->getTexCoordinates().size()*sizeof(float)*2, &material->getTexCoordinates()[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_TEX_COORD);
        glVertexAttribPointer(SHADER_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    const TerrainMaterial *Terrain::getMaterial() const
    {
        return material.get();
    }

    void Terrain::setGrass(std::unique_ptr<TerrainGrass> &terrainGrass)
    {
        if(grass != terrainGrass)
        {
            grass = std::move(terrainGrass);
        }

        grass->initialize(mesh, position);
        grass->onCameraProjectionUpdate(projectionMatrix);
    }

    const TerrainGrass *Terrain::getGrass()
    {
        return grass.get();
    }

    void Terrain::setPosition(const Point3<float> &position)
    { //TODO update grass position
        this->position = position;

        ShaderManager::instance()->bind(terrainShader);
        glUniform3fv(vPositionLoc, 1, (const float*) position);
    }

    const Point3<float> &Terrain::getPosition() const
    {
        return position;
    }

    float Terrain::getAmbient() const
    {
        return ambient;
    }

    void Terrain::setAmbient(float ambient)
    {
        this->ambient = ambient;

        ShaderManager::instance()->bind(terrainShader);
        glUniform1f(ambientLoc, ambient);
    }

    void Terrain::display(const Matrix4<float> &viewMatrix, float invFrameRate) const
    {
        ShaderManager::instance()->bind(terrainShader);

        glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, (const float*)viewMatrix);
        material->loadTextures();

        glBindVertexArray(vertexArrayObject);
        glDrawElements(GL_TRIANGLE_STRIP, mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);

        if(grass!=nullptr)
        {
            grass->display(viewMatrix, invFrameRate);
        }
    }
}
