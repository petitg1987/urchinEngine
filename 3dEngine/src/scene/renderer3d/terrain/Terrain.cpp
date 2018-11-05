#include <GL/glew.h>
#include <cassert>
#include <stdexcept>

#include "Terrain.h"
#include "utils/shader/ShaderManager.h"

#define DEFAULT_AMBIENT 0.3

namespace urchin
{
    Terrain::Terrain(std::shared_ptr<TerrainMesh> &mesh, std::unique_ptr<TerrainMaterial> &material, const Point3<float> &position)
    {
        glGenBuffers(4, bufferIDs);
        glGenVertexArrays(1, &vertexArrayObject);

        terrainShader = ShaderManager::instance()->createProgram("terrain.vert", "", "terrain.frag");
        ShaderManager::instance()->bind(terrainShader);

        vPositionLoc = glGetUniformLocation(terrainShader, "vPosition");
        mProjectionLoc = glGetUniformLocation(terrainShader, "mProjection");
        mViewLoc = glGetUniformLocation(terrainShader, "mView");
        ambientLoc = glGetUniformLocation(terrainShader, "ambient");
        sRepeatLoc = glGetUniformLocation(terrainShader, "sRepeat");
        tRepeatLoc = glGetUniformLocation(terrainShader, "tRepeat");

        int maskTexLoc = glGetUniformLocation(terrainShader, "maskTex");
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(maskTexLoc, 0);

        for(unsigned int i=0; i<MAX_MATERIAL; ++i)
        {
            std::string shaderTextureName = "diffuseTex" + std::to_string(i + 1);
            int diffuseTexLoc = glGetUniformLocation(terrainShader, shaderTextureName.c_str());

            glActiveTexture(GL_TEXTURE0 + i + 1);
            glUniform1i(diffuseTexLoc, i + 1);
        }

        setPosition(position);
        setAmbient(DEFAULT_AMBIENT);
        setMesh(mesh);
        setMaterial(material);

        grass = std::make_unique<TerrainGrass>("");
        refreshGrassMesh();
        refreshGrassAmbient();
    }

    Terrain::~Terrain()
    {
        glDeleteVertexArrays(1, &vertexArrayObject);
        glDeleteBuffers(4, bufferIDs);

        ShaderManager::instance()->removeProgram(terrainShader);
    }

    void Terrain::onCameraProjectionUpdate(const Matrix4<float> &projectionMatrix)
    {
        this->projectionMatrix = projectionMatrix;

        ShaderManager::instance()->bind(terrainShader);
        glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float*)projectionMatrix);

        grass->onCameraProjectionUpdate(projectionMatrix);
    }

    void Terrain::setMesh(const std::shared_ptr<TerrainMesh> &mesh)
    {
        this->mesh = mesh;

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

        refreshMaterial(); //material uses mesh info: refresh is required
        refreshGrassMesh(); //grass uses mesh info: refresh is required
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
        }

        refreshMaterial();
    }

    void Terrain::refreshMaterial()
    {
        if(material)
        {
            material->refreshWith(mesh->getXSize(), mesh->getZSize());

            ShaderManager::instance()->bind(terrainShader);
            glUniform1f(sRepeatLoc, material->getSRepeat());
            glUniform1f(tRepeatLoc, material->getTRepeat());

            glBindVertexArray(vertexArrayObject);
            glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
            glBufferData(GL_ARRAY_BUFFER, material->getTexCoordinates().size() * sizeof(float) * 2, &material->getTexCoordinates()[0], GL_STATIC_DRAW);
            glEnableVertexAttribArray(SHADER_TEX_COORD);
            glVertexAttribPointer(SHADER_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        }
    }

    const TerrainMaterial *Terrain::getMaterial() const
    {
        return material.get();
    }

    void Terrain::refreshGrassMesh()
    {
        if(grass)
        {
            grass->refreshWith(mesh, position);
        }
    }

    void Terrain::refreshGrassAmbient()
    {
        if(grass)
        {
            grass->refreshWith(ambient);
        }
    }

    TerrainGrass *Terrain::getGrass() const
    {
        return grass.get();
    }

    void Terrain::setPosition(const Point3<float> &position)
    {
        this->position = position;

        ShaderManager::instance()->bind(terrainShader);
        glUniform3fv(vPositionLoc, 1, (const float*) position);
        refreshGrassMesh(); //grass uses terrain position: refresh is required
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
        refreshGrassAmbient(); //grass uses ambient value: refresh is required
    }

    Point3<float> Terrain::findPointAt(const Point2<float> &globalXzCoordinate) const
    {
        Point2<float> localCoordinate = Point2<float>(globalXzCoordinate.X - position.X, globalXzCoordinate.Y - position.Z);
        return mesh->findPointAt(localCoordinate) + position;
    }

    float Terrain::findHeightAt(const Point2<float> &globalXzCoordinate) const
    {
        Point2<float> localCoordinate = Point2<float>(globalXzCoordinate.X - position.X, globalXzCoordinate.Y - position.Z);
        return mesh->findHeightAt(localCoordinate) + position.Y;
    }

    void Terrain::display(const Camera *camera, float invFrameRate) const
    {
        ShaderManager::instance()->bind(terrainShader);

        glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, (const float*)camera->getViewMatrix());
        material->loadTextures();

        glBindVertexArray(vertexArrayObject);
        glDrawElements(GL_TRIANGLE_STRIP, mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);

        if(grass)
        {
            grass->display(camera, invFrameRate);
        }
    }
}
