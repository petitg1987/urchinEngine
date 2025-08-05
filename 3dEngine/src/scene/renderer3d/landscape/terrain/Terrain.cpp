#include "scene/renderer3d/landscape/terrain/Terrain.h"
#include "graphics/render/shader/ShaderBuilder.h"
#include "graphics/render/GenericRendererBuilder.h"

namespace urchin {

    /**
     * @param position Terrain position. Position is centered on XZ axis and Y value represents a point without elevation.
     */
    Terrain::Terrain(std::unique_ptr<TerrainMesh> mesh, std::unique_ptr<TerrainMaterials> materials, const Point3<float>& position) :
            isInitialized(false),
            renderTarget(nullptr),
            mesh(std::move(mesh)),
            materials(std::move(materials)),
            grass(TerrainGrass("")),
            ambient(0.0f),
            lightMask(std::numeric_limits<uint8_t>::max()) {
        setPosition(position);
        setAmbient(0.3f);
    }

    void Terrain::initialize(RenderTarget& renderTarget) {
        assert(!isInitialized);
        this->renderTarget = &renderTarget;

        grass.initialize(renderTarget);

        setMesh(std::move(mesh));
        setMaterials(std::move(materials));

        refreshGrassMesh();
        refreshGrassAmbient();

        isInitialized = true;
    }

    void Terrain::setMesh(std::unique_ptr<TerrainMesh> mesh) {
        assert(renderTarget);
        this->mesh = std::move(mesh);

        createOrUpdateRenderer();
    }

    void Terrain::createOrUpdateRenderer() {
        std::vector<Point2<float>> dummyTextureCoordinates;
        dummyTextureCoordinates.resize(mesh->getVertices().size(), Point2(0.0f, 0.0f));

        Matrix4<float> projViewMatrix;
        Vector2<float> materialsStRepeat = materials->getStRepeat();

        TerrainShaderConst terrainShaderConst {
            .ambient = ambient,
            .lightMask = lightMask
        };
        std::vector variablesSize = {
            sizeof(TerrainShaderConst::ambient),
            sizeof(TerrainShaderConst::lightMask)
        };
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &terrainShaderConst);
        if (mesh->getMode() == TerrainMeshMode::SMOOTH) {
            terrainShader = ShaderBuilder::createShader("terrainShadeSmooth.vert.spv", "terrainShadeSmooth.frag.spv", std::move(shaderConstants), false);
        } else {
            assert(mesh->getMode() == TerrainMeshMode::FLAT);
            terrainShader = ShaderBuilder::createShader("terrainShadeFlat.vert.spv", "terrainShadeFlat.frag.spv", std::move(shaderConstants), false);
        }

        auto terrainRendererBuilder = GenericRendererBuilder::create("terrain", *renderTarget, *terrainShader, ShapeType::TRIANGLE_STRIP)
                ->enableDepthTest()
                ->enableDepthWrite()
                ->addData(mesh->getVertices())
                ->addData(dummyTextureCoordinates);

        if (mesh->getMode() == TerrainMeshMode::SMOOTH) {
            terrainRendererBuilder->addData(mesh->getNormals());
        }

        terrainRendererBuilder
                ->indices(mesh->getIndices())
                ->addUniformData(PROJ_VIEW_MATRIX_UNIFORM_BINDING, sizeof(projViewMatrix), &projViewMatrix)
                ->addUniformData(POSITION_UNIFORM_BINDING, sizeof(position), &position)
                ->addUniformData(ST_UNIFORM_BINDING, sizeof(materialsStRepeat), &materialsStRepeat)
                ->addUniformTextureReader(MASK_TEX_UNIFORM_BINDING, TextureReader::build(Texture::buildEmptyRgba("terrain empty mask"), TextureParam::buildNearest())); //mask texture

        for (std::size_t i = 0; i < materials->getMaterials().size(); ++i) {
            terrainRendererBuilder->addUniformTextureReader(MATERIAL_TEX_UNIFORM_BINDING[i], TextureReader::build(Texture::buildEmptyRgba("terrain empty material"), TextureParam::buildNearest()));
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

    void Terrain::refreshMaterials() const {
        if (materials) {
            materials->refreshWith(mesh->getXSize(), mesh->getZSize());

            Vector2<float> materialsStRepeat = materials->getStRepeat();
            terrainRenderer->updateData(1, materials->getTexCoordinates());
            terrainRenderer->updateUniformData(ST_UNIFORM_BINDING, &materialsStRepeat);
            terrainRenderer->updateUniformTextureReader(MASK_TEX_UNIFORM_BINDING, TextureReader::build(materials->getMaskTexture(), TextureParam::buildLinear()));
            for (std::size_t i = 0; i < materials->getMaterials().size(); ++i) {
                const auto& material = materials->getMaterials()[i];
                if (material) {
                    TextureParam::ReadMode textureReadMode = material->repeatTextures() ? TextureParam::ReadMode::REPEAT : TextureParam::ReadMode::EDGE_CLAMP;
                    TextureParam textureParam = TextureParam::build(textureReadMode, TextureParam::LINEAR, TextureParam::ANISOTROPY);
                    terrainRenderer->updateUniformTextureReader(MATERIAL_TEX_UNIFORM_BINDING[i], TextureReader::build(material->getAlbedoTexture(), std::move(textureParam)));
                } else {
                    terrainRenderer->updateUniformTextureReader(MATERIAL_TEX_UNIFORM_BINDING[i], TextureReader::build(Texture::buildEmptyRgba("terrain empty material"), TextureParam::buildNearest()));
                }
            }
        }
    }

    const TerrainMaterials* Terrain::getMaterials() const {
        return materials.get();
    }

    void Terrain::refreshGrassMesh() {
        if (grass.isInitialized()) {
            grass.refreshWith(mesh.get(), position);
        }
    }

    void Terrain::refreshGrassAmbient() {
        if (grass.isInitialized()) {
            grass.refreshWith(ambient);
        }
    }

    TerrainGrass& Terrain::getGrass() {
        return grass;
    }

    const TerrainGrass& Terrain::getGrass() const {
        return grass;
    }

    /**
     * @param position Terrain position. Position is centered on XZ axis and Y value represents a point without elevation.
     */
    void Terrain::setPosition(const Point3<float>& position) {
        this->position = position;

        if (terrainRenderer) {
            terrainRenderer->updateUniformData(POSITION_UNIFORM_BINDING, &position);
        }
        refreshGrassMesh(); //grass uses terrain position: refresh is required
    }

    /**
     * @return Terrain position. Position is centered on XZ axis and Y value represents a point without elevation.
     */
    const Point3<float>& Terrain::getPosition() const {
        return position;
    }

    float Terrain::getAmbient() const {
        return ambient;
    }

    void Terrain::setAmbient(float ambient) {
        if (this->ambient != ambient) {
            this->ambient = ambient;

            if (isInitialized) {
                createOrUpdateRenderer();
                refreshGrassAmbient(); //grass uses ambient value: refresh is required
            }
        }
    }

    void Terrain::setLightMask(uint8_t lightMask) {
        if (this->lightMask != lightMask) {
            this->lightMask = lightMask;

            if (isInitialized) {
                createOrUpdateRenderer();
                //TODO refrehs grass !
            }
        }
    }

    uint8_t Terrain::getLightMask() const {
        return lightMask;
    }

    Point3<float> Terrain::findPointAt(const Point2<float>& globalXzCoordinate) const {
        Point2 localCoordinate(globalXzCoordinate.X - position.X, globalXzCoordinate.Y - position.Z);
        return mesh->findPointAt(localCoordinate) + position;
    }

    float Terrain::findHeightAt(const Point2<float>& globalXzCoordinate) const {
        Point2 localCoordinate(globalXzCoordinate.X - position.X, globalXzCoordinate.Y - position.Z);
        return mesh->findHeightAt(localCoordinate) + position.Y;
    }

    void Terrain::prepareRendering(unsigned int renderingOrder, const Camera& camera, float dt) {
        assert(isInitialized);

        terrainRenderer->updateUniformData(PROJ_VIEW_MATRIX_UNIFORM_BINDING, &camera.getProjectionViewMatrix());
        terrainRenderer->enableRenderer(renderingOrder);

        grass.prepareRendering(renderingOrder, camera, dt);
    }
}
