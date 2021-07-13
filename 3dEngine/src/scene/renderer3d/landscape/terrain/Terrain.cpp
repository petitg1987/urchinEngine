#include <scene/renderer3d/landscape/terrain/Terrain.h>
#include <graphic/render/shader/builder/ShaderBuilder.h>
#include <graphic/render/GenericRendererBuilder.h>

namespace urchin {

    /**
     * @param position Terrain position. Position is centered on XZ axis and Y value represents a point without elevation.
     */
    Terrain::Terrain(std::unique_ptr<TerrainMesh> mesh, std::unique_ptr<TerrainMaterials> materials, const Point3<float>& position) :
            isInitialized(false),
            renderTarget(nullptr),
            mesh(std::move(mesh)),
            materials(std::move(materials)),
            grass(std::make_unique<TerrainGrass>("")),
            ambient(0.0f) {
        terrainShader = ShaderBuilder::createShader("terrain.vert.spv", "", "terrain.frag.spv", std::unique_ptr<ShaderConstants>());

        setPosition(position);
        setAmbient(0.3f);
    }

    void Terrain::initialize(RenderTarget& renderTarget) {
        assert(!isInitialized);
        this->renderTarget = &renderTarget;

        grass->initialize(renderTarget);

        setMesh(std::move(mesh));
        setMaterials(std::move(materials));

        refreshGrassMesh();
        refreshGrassAmbient();

        isInitialized = true;
    }

    void Terrain::onCameraProjectionUpdate(const Matrix4<float>& projectionMatrix) {
        positioningData.projectionMatrix = projectionMatrix;

        terrainRenderer->updateUniformData(1, &positioningData);
        grass->onCameraProjectionUpdate(projectionMatrix);
    }

    void Terrain::setMesh(std::unique_ptr<TerrainMesh> mesh) {
        assert(renderTarget);
        this->mesh = std::move(mesh);

        std::vector<Point2<float>> dummyTextureCoordinates;
        dummyTextureCoordinates.resize(this->mesh->getVertices().size(), Point2<float>(0.0f, 0.0f));

        Matrix4<float> viewMatrix;
        Vector2<float> materialsStRepeat = materials->getStRepeat();

        auto terrainRendererBuilder = GenericRendererBuilder::create("terrain", *renderTarget, *terrainShader, ShapeType::TRIANGLE_STRIP)
                ->enableDepthOperations()
                ->addData(this->mesh->getVertices())
                ->addData(this->mesh->getNormals())
                ->addData(dummyTextureCoordinates)
                ->indices(this->mesh->getIndices())
                ->addUniformData(sizeof(viewMatrix), &viewMatrix) //binding 0
                ->addUniformData(sizeof(positioningData), &positioningData) //binding 1
                ->addUniformData(sizeof(materialsStRepeat), &materialsStRepeat) //binding 2
                ->addUniformData(sizeof(ambient), &ambient) //binding 3
                ->addUniformTextureReader(TextureReader::build(Texture::buildEmptyRgba(), TextureParam::buildNearest())); //binding 4 - mask texture
        for (std::size_t i = 0; i < materials->getMaterials().size(); ++i) {
            terrainRendererBuilder->addUniformTextureReader(TextureReader::build(Texture::buildEmptyRgba(), TextureParam::buildNearest())); //binding 5..8 - material texture
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
            terrainRenderer->updateUniformData(2, &materialsStRepeat);

            std::size_t maskMaterialTexUnit = 0;
            std::size_t materialTexUnitStart = 1;

            terrainRenderer->updateUniformTextureReader(maskMaterialTexUnit, TextureReader::build(materials->getMaskTexture(), TextureParam::buildLinear()));
            for (auto& material : materials->getMaterials()) {
                if (material) {
                    TextureParam::ReadMode textureReadMode = material->isRepeatableTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
                    TextureParam textureParam = TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);
                    terrainRenderer->updateUniformTextureReader(materialTexUnitStart++, TextureReader::build(material->getDiffuseTexture(), textureParam));
                } else {
                    terrainRenderer->updateUniformTextureReader(materialTexUnitStart++, TextureReader::build(Texture::buildEmptyRgba(), TextureParam::buildNearest()));
                }
            }
        }
    }

    const TerrainMaterials* Terrain::getMaterials() const {
        return materials.get();
    }

    void Terrain::refreshGrassMesh() {
        if (grass->isInitialized()) {
            grass->refreshWith(mesh.get(), positioningData.position);
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
            terrainRenderer->updateUniformData(1, &positioningData);
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

        if (terrainRenderer) {
            terrainRenderer->updateUniformData(3, &ambient);
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

    void Terrain::prepareRendering(const Camera* camera, float dt) const {
        assert(isInitialized);

        terrainRenderer->updateUniformData(0, &camera->getViewMatrix());
        terrainRenderer->enableRenderer();

        if (grass) {
            grass->prepareRendering(camera, dt);
        }
    }
}
