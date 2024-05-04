#include <stdexcept>

#include <scene/renderer3d/landscape/sky/Skybox.h>
#include <resources/ResourceRetriever.h>
#include <graphics/render/GenericRendererBuilder.h>
#include <graphics/render/shader/ShaderBuilder.h>

namespace urchin {

    /**
     * @param filenames Filenames of the textures in the following order: X- (left), X+ (right), Y- (bottom), Y+ (top), Z- (front), Z+ (back)
     */
    Skybox::Skybox(const std::vector<std::string>& filenames) :
            isInitialized(false),
            offsetY(0.0f) {
        if (filenames.size() != 6) {
            throw std::invalid_argument("There is no 6 skybox filenames.");
        }
        this->filenames.reserve(filenames.size());
        for (const std::string& filename : filenames) {
            if (filename.empty() || FileUtil::isAbsolutePath(filename)) {
                this->filenames.push_back(filename);
            } else {
                this->filenames.push_back(FileSystem::instance().getResourcesDirectory() + filename);
            }
        }

        //create the textures
        std::vector<std::jthread> loadImageThreads(6);
        for (std::size_t i = 0; i < skyboxImages.size(); ++i) {
            const std::string& filename = filenames[i];
            loadImageThreads[i] = std::jthread([i, filename, this]() {
                if (!filename.empty()) {
                    skyboxImages[i] = ResourceRetriever::instance().getResource<Image>(filename);
                }
            });
        }
        std::ranges::for_each(loadImageThreads, [](std::jthread& x){ x.join(); });

        //add missing default textures
        unsigned int skyboxSize = 1;
        for (const auto& skyboxImage : skyboxImages) {
            if (skyboxImage) {
                skyboxSize = skyboxImage->getWidth();
                break;
            }
        }
        for (std::size_t i = 0; i < skyboxImages.size(); ++i) {
            if (filenames[i].empty()) {
                std::vector<unsigned char> defaultTexPixels(skyboxSize * skyboxSize * 4, 0);
                skyboxImages[i] = std::make_shared<Image>(skyboxSize, skyboxSize, Image::IMAGE_RGBA, std::move(defaultTexPixels), false);
            }
        }

        //check consistency
        for (std::size_t i = 0; i < 6 - 1; ++i) {
            unsigned int widthSize = skyboxImages[i]->getWidth();
            unsigned int heightSize = skyboxImages[i]->getHeight();
            TextureFormat textureFormat = skyboxImages[0]->retrieveTextureFormat();

            unsigned int nextWidthSize = skyboxImages[i + 1]->getWidth();
            TextureFormat nextTextureFormat = skyboxImages[i + 1]->retrieveTextureFormat();

            if (skyboxImages[i]->getWidth() != skyboxImages[i]->getHeight()) {
                throw std::runtime_error("Skybox image must be a square. Present image size: " + std::to_string(widthSize) + "x" + std::to_string(heightSize));
            } else if (widthSize != nextWidthSize) {
                throw std::runtime_error("All skybox images must have the same size: " + std::to_string(widthSize) + " != " + std::to_string(nextWidthSize));
            } else if (textureFormat != nextTextureFormat) {
                throw std::runtime_error("All skybox images must have the same texture format: " + std::to_string((int)textureFormat) + " != " + std::to_string((int)nextTextureFormat));
            }
        }
    }

    void Skybox::initialize(RenderTarget& renderTarget) {
        if (isInitialized && &skyboxRenderer->getRenderTarget() == &renderTarget) {
            return;
        }

        //texture creation
        std::vector<const void*> cubeDataPtr = {skyboxImages[0]->getTexels().data(), skyboxImages[1]->getTexels().data(), skyboxImages[2]->getTexels().data(),
                                                skyboxImages[3]->getTexels().data(), skyboxImages[4]->getTexels().data(), skyboxImages[5]->getTexels().data()};
        auto skyboxTexture = Texture::buildCubeMap("skybox", skyboxImages[0]->getWidth(), skyboxImages[0]->getHeight(), skyboxImages[0]->retrieveTextureFormat(), cubeDataPtr, TextureDataType::INT_8);
        std::ranges::fill(skyboxImages, std::shared_ptr<Image>(nullptr));

        //visual
        skyboxShader = ShaderBuilder::createShader("skybox.vert.spv", "", "skybox.frag.spv");

        constexpr float SIZE = 10.0f;
        std::vector<Point3<float>> vertexCoord = {
            //x negative:
            Point3<float>(-SIZE, -SIZE, SIZE), Point3<float>(-SIZE, SIZE, SIZE), Point3<float>(-SIZE, SIZE, -SIZE),
            Point3<float>(-SIZE, -SIZE, SIZE), Point3<float>(-SIZE, SIZE, -SIZE), Point3<float>(-SIZE, -SIZE, -SIZE),
            //x positive:
            Point3<float>(SIZE, -SIZE, -SIZE), Point3<float>(SIZE, SIZE, -SIZE), Point3<float>(SIZE, SIZE, SIZE),
            Point3<float>(SIZE, -SIZE, -SIZE), Point3<float>(SIZE, SIZE, SIZE), Point3<float>(SIZE, -SIZE, SIZE),
            //y negative:
            Point3<float>(-SIZE, -SIZE, -SIZE), Point3<float>(SIZE, -SIZE, -SIZE), Point3<float>(SIZE, -SIZE, SIZE),
            Point3<float>(-SIZE, -SIZE, -SIZE), Point3<float>(SIZE, -SIZE, SIZE), Point3<float>(-SIZE, -SIZE, SIZE),
            //y positive:
            Point3<float>(-SIZE, SIZE, SIZE), Point3<float>(SIZE, SIZE, SIZE), Point3<float>(SIZE, SIZE, -SIZE),
            Point3<float>(-SIZE, SIZE, SIZE), Point3<float>(SIZE, SIZE, -SIZE), Point3<float>(-SIZE, SIZE, -SIZE),
            //z negative:
            Point3<float>(-SIZE, -SIZE, -SIZE), Point3<float>(-SIZE, SIZE, -SIZE), Point3<float>(SIZE, SIZE, -SIZE),
            Point3<float>(-SIZE, -SIZE, -SIZE), Point3<float>(SIZE, SIZE, -SIZE), Point3<float>(SIZE, -SIZE, -SIZE),
            //z positive:
            Point3<float>(SIZE, -SIZE, SIZE), Point3<float>(SIZE, SIZE, SIZE), Point3<float>(-SIZE, SIZE, SIZE),
            Point3<float>(SIZE, -SIZE, SIZE), Point3<float>(-SIZE, SIZE, SIZE), Point3<float>(-SIZE, -SIZE, SIZE),
        };

        std::vector<Point3<float>> textureCoord = {
            //x negative:
            Point3<float>(SIZE, -SIZE, SIZE), Point3<float>(SIZE, SIZE, SIZE), Point3<float>(SIZE, SIZE, -SIZE),
            Point3<float>(SIZE, -SIZE, SIZE), Point3<float>(SIZE, SIZE, -SIZE), Point3<float>(SIZE, -SIZE, -SIZE),
            //x positive:
            Point3<float>(-SIZE, -SIZE, -SIZE), Point3<float>(-SIZE, SIZE, -SIZE), Point3<float>(-SIZE, SIZE, SIZE),
            Point3<float>(-SIZE, -SIZE, -SIZE), Point3<float>(-SIZE, SIZE, SIZE), Point3<float>(-SIZE, -SIZE, SIZE),
            //y negative:
            Point3<float>(-SIZE, SIZE, -SIZE), Point3<float>(SIZE, SIZE, -SIZE), Point3<float>(SIZE, SIZE, SIZE),
            Point3<float>(-SIZE, SIZE, -SIZE), Point3<float>(SIZE, SIZE, SIZE), Point3<float>(-SIZE, SIZE, SIZE),
            //y positive:
            Point3<float>(-SIZE, -SIZE, SIZE), Point3<float>(SIZE, -SIZE, SIZE), Point3<float>(SIZE, -SIZE, -SIZE),
            Point3<float>(-SIZE, -SIZE, SIZE), Point3<float>(SIZE, -SIZE, -SIZE), Point3<float>(-SIZE, -SIZE, -SIZE),
            //z negative:
            Point3<float>(-SIZE, -SIZE, SIZE), Point3<float>(-SIZE, SIZE, SIZE), Point3<float>(SIZE, SIZE, SIZE),
            Point3<float>(-SIZE, -SIZE, SIZE), Point3<float>(SIZE, SIZE, SIZE), Point3<float>(SIZE, -SIZE, SIZE),
            //z positive:
            Point3<float>(SIZE, -SIZE, -SIZE), Point3<float>(SIZE, SIZE, -SIZE), Point3<float>(-SIZE, SIZE, -SIZE),
            Point3<float>(SIZE, -SIZE, -SIZE), Point3<float>(-SIZE, SIZE, -SIZE), Point3<float>(-SIZE, -SIZE, -SIZE)
        };

        Matrix4<float> projectionViewMatrix;
        skyboxRenderer = GenericRendererBuilder::create("skybox", renderTarget, *skyboxShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(PROJ_VIEW_UNIFORM_BINDING, sizeof(projectionViewMatrix), &projectionViewMatrix)
                ->addUniformTextureReader(SKYBOX_TEX_UNIFORM_BINDING, TextureReader::build(skyboxTexture, TextureParam::buildNearest()))
                ->build();

        this->isInitialized = true;
    }

    float Skybox::getOffsetY() const {
        return offsetY;
    }

    void Skybox::setOffsetY(float offsetY) {
        this->offsetY = offsetY;
    }

    const std::vector<std::string>& Skybox::getFilenames() const {
        return filenames;
    }

    void Skybox::prepareRendering(unsigned int renderingOrder, const Matrix4<float>& projectionViewMatrix, const Point3<float>& cameraPosition) {
        assert(isInitialized);
        translationMatrix = Matrix4<float>::buildTranslation(cameraPosition.X, cameraPosition.Y + offsetY, cameraPosition.Z);
        Matrix4<float> skyboxProjectionViewMatrix = projectionViewMatrix * translationMatrix;
        skyboxRenderer->updateUniformData(PROJ_VIEW_UNIFORM_BINDING, &skyboxProjectionViewMatrix);

        skyboxRenderer->enableRenderer(renderingOrder);
    }

}
