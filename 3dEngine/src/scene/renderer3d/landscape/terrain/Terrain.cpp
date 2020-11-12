#include <GL/glew.h>
#include <stdexcept>

#include "Terrain.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"

#define DEFAULT_AMBIENT 0.3f

namespace urchin {

    /**
     * @param position Terrain position. Position is centered on XZ axis and Y value represents a point without elevation.
     */
    Terrain::Terrain(std::shared_ptr<TerrainMesh> &mesh, std::unique_ptr<TerrainMaterial> &material, const Point3<float> &position) :
            bufferIDs(),
            vertexArrayObject(0),
            ambient(0.0f) {
        glGenBuffers(4, bufferIDs);
        glGenVertexArrays(1, &vertexArrayObject);

        terrainShader = ShaderBuilder().createShader("terrain.vert", "", "terrain.frag");

        vPositionShaderVar = ShaderVar(terrainShader, "vPosition");
        mProjectionShaderVar = ShaderVar(terrainShader, "mProjection");
        mViewShaderVar = ShaderVar(terrainShader, "mView");
        ambientShaderVar = ShaderVar(terrainShader, "ambient");
        sRepeatShaderVar = ShaderVar(terrainShader, "sRepeat");
        tRepeatShaderVar = ShaderVar(terrainShader, "tRepeat");

        int maskTexUnit = 0;
        ShaderDataSender().sendData(ShaderVar(terrainShader, "maskTex"), maskTexUnit);

        for (int i=0; i<MAX_MATERIAL; ++i) {
            std::string shaderTextureName = "diffuseTex" + std::to_string(i + 1);
            int diffuseTexUnit = i + 1;
            ShaderDataSender().sendData(ShaderVar(terrainShader, std::move(shaderTextureName)), diffuseTexUnit);
        }

        setPosition(position);
        setAmbient(DEFAULT_AMBIENT);
        setMesh(mesh);
        setMaterial(material);

        grass = std::make_unique<TerrainGrass>("");
        refreshGrassMesh();
        refreshGrassAmbient();
    }

    Terrain::~Terrain() {
        glDeleteVertexArrays(1, &vertexArrayObject);
        glDeleteBuffers(4, bufferIDs);
    }

    void Terrain::onCameraProjectionUpdate(const Matrix4<float> &projectionMatrix) {
        this->projectionMatrix = projectionMatrix;

        ShaderDataSender().sendData(mProjectionShaderVar, projectionMatrix);
        grass->onCameraProjectionUpdate(projectionMatrix);
    }

    void Terrain::setMesh(const std::shared_ptr<TerrainMesh> &mesh) {
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

    const TerrainMesh *Terrain::getMesh() const {
        return mesh.get();
    }

    void Terrain::setMaterial(std::unique_ptr<TerrainMaterial> &terrainMaterial) {
        if (material != terrainMaterial) {
            material = std::move(terrainMaterial);
        }

        refreshMaterial();
    }

    void Terrain::refreshMaterial() {
        if (material) {
            material->refreshWith(mesh->getXSize(), mesh->getZSize());

            ShaderDataSender()
                .sendData(sRepeatShaderVar, material->getSRepeat())
                .sendData(tRepeatShaderVar, material->getTRepeat());

            glBindVertexArray(vertexArrayObject);
            glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
            glBufferData(GL_ARRAY_BUFFER, material->getTexCoordinates().size() * sizeof(float) * 2, &material->getTexCoordinates()[0], GL_STATIC_DRAW);
            glEnableVertexAttribArray(SHADER_TEX_COORD);
            glVertexAttribPointer(SHADER_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        }
    }

    const TerrainMaterial *Terrain::getMaterial() const {
        return material.get();
    }

    void Terrain::refreshGrassMesh() {
        if (grass) {
            grass->refreshWith(mesh, position);
        }
    }

    void Terrain::refreshGrassAmbient() {
        if (grass) {
            grass->refreshWith(ambient);
        }
    }

    TerrainGrass *Terrain::getGrass() const {
        return grass.get();
    }

    /**
     * @param position Terrain position. Position is centered on XZ axis and Y value represents a point without elevation.
     */
    void Terrain::setPosition(const Point3<float> &position) {
        this->position = position;

        ShaderDataSender().sendData(vPositionShaderVar, position);
        refreshGrassMesh(); //grass uses terrain position: refresh is required
    }

    /**
     * @return Terrain position. Position is centered on XZ axis and Y value represents a point without elevation.
     */
    const Point3<float> &Terrain::getPosition() const {
        return position;
    }

    float Terrain::getAmbient() const {
        return ambient;
    }

    void Terrain::setAmbient(float ambient) {
        this->ambient = ambient;

        ShaderDataSender().sendData(ambientShaderVar, ambient);
        refreshGrassAmbient(); //grass uses ambient value: refresh is required
    }

    Point3<float> Terrain::findPointAt(const Point2<float> &globalXzCoordinate) const {
        Point2<float> localCoordinate = Point2<float>(globalXzCoordinate.X - position.X, globalXzCoordinate.Y - position.Z);
        return mesh->findPointAt(localCoordinate) + position;
    }

    float Terrain::findHeightAt(const Point2<float> &globalXzCoordinate) const {
        Point2<float> localCoordinate = Point2<float>(globalXzCoordinate.X - position.X, globalXzCoordinate.Y - position.Z);
        return mesh->findHeightAt(localCoordinate) + position.Y;
    }

    void Terrain::display(const Camera *camera, float dt) const {
        ShaderDataSender().sendData(mViewShaderVar, camera->getViewMatrix());
        material->loadTextures();

        terrainShader->bind();
        glBindVertexArray(vertexArrayObject);
        glDrawElements(GL_TRIANGLE_STRIP, mesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);

        if (grass) {
            grass->display(camera, dt);
        }
    }
}
