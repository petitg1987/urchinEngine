#include "Terrain.h"
#include "graphic/render/shader/builder/ShaderBuilder.h"
#include "graphic/render/GenericRendererBuilder.h"

#define DEFAULT_AMBIENT 0.3f

namespace urchin {

    /**
     * @param position Terrain position. Position is centered on XZ axis and Y value represents a point without elevation.
     */
    Terrain::Terrain(std::shared_ptr<TerrainMesh>& mesh, std::unique_ptr<TerrainMaterials> materials, const Point3<float>& position) :
            isInitialized(false),
            mesh(mesh),
            materials(std::move(materials)),
            grass(std::make_unique<TerrainGrass>("")),
            ambient(0.0f) {
        terrainShader = ShaderBuilder::createShader("terrain.vert", "", "terrain.frag");

        setPosition(position);
        setAmbient(DEFAULT_AMBIENT);
    }

    void Terrain::initialize(std::shared_ptr<RenderTarget> renderTarget) {
        assert(!isInitialized);
        this->renderTarget = std::move(renderTarget);

        grass->initialize(this->renderTarget);

        setMesh(mesh);
        setMaterials(std::move(materials));

        refreshGrassMesh();
        refreshGrassAmbient();

        isInitialized = true;
    }

    void Terrain::onCameraProjectionUpdate(const Matrix4<float>& projectionMatrix) {
        positioningData.projectionMatrix = projectionMatrix;

        terrainRenderer->updateShaderData(1, &positioningData);
        grass->onCameraProjectionUpdate(projectionMatrix);
    }

    void Terrain::setMesh(const std::shared_ptr<TerrainMesh>& mesh) {
        assert(renderTarget);
        this->mesh = mesh;

        std::vector<Point2<float>> emptyTextureCoordinates;
        Matrix4<float> viewMatrix;
        Vector2<float> materialsStRepeat = materials->getStRepeat();

        auto terrainRendererBuilder = std::make_unique<GenericRendererBuilder>(renderTarget, terrainShader, ShapeType::TRIANGLE_STRIP);
        terrainRendererBuilder
                ->enableDepthOperations()
                ->addData(mesh->getVertices())
                ->addData(mesh->getNormals())
                ->addData(emptyTextureCoordinates)
                ->indices(mesh->getIndices())
                ->addShaderData(sizeof(viewMatrix), &viewMatrix) //binding 0
                ->addShaderData(sizeof(positioningData), &positioningData) //binding 1
                ->addShaderData(sizeof(materials->getStRepeat()), &materialsStRepeat) //binding 2
                ->addShaderData(sizeof(ambient), &ambient) //binding 3
                ->addTextureReader(TextureReader::build(Texture::buildEmpty(), TextureParam::buildNearest())); //mask texture
        for (std::size_t i = 0; i < materials->getMaterials().size(); ++i) {
            terrainRendererBuilder->addTextureReader(TextureReader::build(Texture::buildEmpty(), TextureParam::buildNearest())); //material texture
        }
        terrainRenderer = terrainRendererBuilder->build();

        refreshMaterials(); //material uses mesh info: refresh is required
        refreshGrassMesh(); //grass uses mesh info: refresh is required
    }

    const TerrainMesh* Terrain::getMesh() const {
        return mesh.get();
    }

    void Terrain::setMaterials(std::unique_ptr<TerrainMaterials> materials) {
        if (this->materials != materials) {
            this->materials = std::move(materials);
        }

        refreshMaterials();
    }

    void Terrain::refreshMaterials() {
        if (materials) {
            materials->refreshWith(mesh->getXSize(), mesh->getZSize());

            Vector2<float> materialsStRepeat = materials->getStRepeat();
            terrainRenderer->updateData(2, materials->getTexCoordinates());
            terrainRenderer->updateShaderData(2, &materialsStRepeat);

            std::size_t maskMaterialTexUnit = 0;
            std::size_t materialTexUnitStart = 1;

            terrainRenderer->updateTextureReader(maskMaterialTexUnit, TextureReader::build(materials->getMaskTexture(), TextureParam::buildLinear()));
            for (auto& material : materials->getMaterials()) {
                if (material) {
                    TextureParam::ReadMode textureReadMode = material->isRepeatableTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
                    TextureParam textureParam = TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);
                    terrainRenderer->updateTextureReader(materialTexUnitStart++, TextureReader::build(material->getDiffuseTexture(), textureParam));
                } else {
                    terrainRenderer->updateTextureReader(materialTexUnitStart++, TextureReader::build(Texture::buildEmpty(), TextureParam::buildNearest()));
                }
            }
        }
    }

    const TerrainMaterials* Terrain::getMaterials() const {
        return materials.get();
    }

    void Terrain::refreshGrassMesh() {
        if (grass->isInitialized()) {
            grass->refreshWith(mesh, positioningData.position);
        }
    }

    void Terrain::refreshGrassAmbient() {
        if (grass->isInitialized()) {
            grass->refreshWith(ambient);
        }
    }

    TerrainGrass* Terrain::getGrass() const {
        return grass.get();
    }

    /**
     * @param position Terrain position. Position is centered on XZ axis and Y value represents a point without elevation.
     */
    void Terrain::setPosition(const Point3<float>& position) {
        positioningData.position = position;

        if (terrainRenderer) {
            terrainRenderer->updateShaderData(1, &positioningData);
        }
        refreshGrassMesh(); //grass uses terrain position: refresh is required
    }

    /**
     * @return Terrain position. Position is centered on XZ axis and Y value represents a point without elevation.
     */
    const Point3<float>& Terrain::getPosition() const {
        return positioningData.position;
    }

    float Terrain::getAmbient() const {
        return ambient;
    }

    void Terrain::setAmbient(float ambient) {
        this->ambient = ambient;

        if(terrainRenderer) {
            terrainRenderer->updateShaderData(3, &ambient);
        }
        refreshGrassAmbient(); //grass uses ambient value: refresh is required
    }

    Point3<float> Terrain::findPointAt(const Point2<float>& globalXzCoordinate) const {
        Point2<float> localCoordinate = Point2<float>(globalXzCoordinate.X - positioningData.position.X, globalXzCoordinate.Y - positioningData.position.Z);
        return mesh->findPointAt(localCoordinate) + positioningData.position;
    }

    float Terrain::findHeightAt(const Point2<float>& globalXzCoordinate) const {
        Point2<float> localCoordinate = Point2<float>(globalXzCoordinate.X - positioningData.position.X, globalXzCoordinate.Y - positioningData.position.Z);
        return mesh->findHeightAt(localCoordinate) + positioningData.position.Y;
    }

    void Terrain::display(const Camera* camera, float dt) const {
        assert(isInitialized);

        terrainRenderer->updateShaderData(0, &camera->getViewMatrix());
        //TODO renderTarget->display(terrainRenderer);

        if (grass) {
            grass->display(camera, dt);
        }
    }
}
