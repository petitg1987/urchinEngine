#include <stdexcept>

#include "Terrain.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "graphic/render/GenericRendererBuilder.h"

#define DEFAULT_AMBIENT 0.3f

namespace urchin {

    /**
     * @param position Terrain position. Position is centered on XZ axis and Y value represents a point without elevation.
     */
    Terrain::Terrain(std::shared_ptr<TerrainMesh> &mesh, std::unique_ptr<TerrainMaterial> &material, const Point3<float>& position) :
            ambient(0.0f) {
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

    void Terrain::onCameraProjectionUpdate(const Matrix4<float>& projectionMatrix) {
        this->projectionMatrix = projectionMatrix;

        ShaderDataSender().sendData(mProjectionShaderVar, projectionMatrix);
        grass->onCameraProjectionUpdate(projectionMatrix);
    }

    void Terrain::setMesh(const std::shared_ptr<TerrainMesh> &mesh) {
        this->mesh = mesh;

        std::vector<Point2<float>> emptyTextureCoordinates;
        terrainRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::TRIANGLE_STRIP)
                ->enableDepthTest()
                ->addData(&mesh->getVertices())
                ->addData(&mesh->getNormals())
                ->addData(&emptyTextureCoordinates)
                ->indices(&mesh->getIndices())
                ->build();

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

            terrainRenderer->updateData(2, &material->getTexCoordinates());

            terrainRenderer->clearAdditionalTextures();
            terrainRenderer->addAdditionalTexture(TextureReader::build(material->getMaskTexture(), TextureParam::buildLinear()));
            for(auto &material : material->getMaterials()) {
                if (material) {
                    TextureParam::ReadMode textureReadMode = material->isRepeatableTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
                    TextureParam textureParam = TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);
                    terrainRenderer->addAdditionalTexture(TextureReader::build(material->getDiffuseTexture(), textureParam));
                }
            }
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
    void Terrain::setPosition(const Point3<float>& position) {
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

    Point3<float> Terrain::findPointAt(const Point2<float>& globalXzCoordinate) const {
        Point2<float> localCoordinate = Point2<float>(globalXzCoordinate.X - position.X, globalXzCoordinate.Y - position.Z);
        return mesh->findPointAt(localCoordinate) + position;
    }

    float Terrain::findHeightAt(const Point2<float>& globalXzCoordinate) const {
        Point2<float> localCoordinate = Point2<float>(globalXzCoordinate.X - position.X, globalXzCoordinate.Y - position.Z);
        return mesh->findHeightAt(localCoordinate) + position.Y;
    }

    void Terrain::display(const RenderTarget* renderTarget, const Camera* camera, float dt) const {
        ShaderDataSender().sendData(mViewShaderVar, camera->getViewMatrix());

        renderTarget->activeShader(terrainShader);
        renderTarget->display(terrainRenderer);

        if (grass) {
            grass->display(renderTarget, camera, dt);
        }
    }
}
