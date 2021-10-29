#include <stdexcept>

#include <scene/renderer3d/landscape/sky/Skybox.h>
#include <resources/ResourceRetriever.h>
#include <api/render/GenericRendererBuilder.h>
#include <api/texture/Texture.h>
#include <api/render/shader/builder/ShaderBuilder.h>

namespace urchin {

    /**
     * @param filenames Filenames of the textures in the following order: X- (left), X+ (right), Y- (bottom), Y+ (top), Z- (front), Z+ (back)
     */
    Skybox::Skybox(const std::vector<std::string>& filenames) :
            isInitialized(false),
            filenames(filenames),
            offsetY(0.0f) {
        if (filenames.size() != 6) {
            throw std::invalid_argument("There is no 6 skybox filenames.");
        }

        //create the textures
        std::vector<std::thread> loadImageThreads(6);
        for (std::size_t i = 0; i < skyboxImages.size(); ++i) {
            std::string filename = filenames[i];
            loadImageThreads[i] = std::thread([i, filename, this]() {
                if (!filename.empty()) {
                    skyboxImages[i] = ResourceRetriever::instance().getResource<Image>(filename);
                }
            });
        }
        std::for_each(loadImageThreads.begin(), loadImageThreads.end(), [](std::thread& x){x.join();});

        //add missing default textures
        unsigned int skyboxSize = 1;
        for (auto& skyboxImage : skyboxImages) {
            if (skyboxImage) {
                skyboxSize = skyboxImage->getWidth();
                break;
            }
        }
        for (std::size_t i = 0; i < skyboxImages.size(); ++i) {
            if (filenames[i].empty()) {
                std::vector<unsigned char> defaultTexPixels;
                defaultTexPixels.reserve(skyboxSize * skyboxSize * 4);
                for (std::size_t pixelIndex = 0; pixelIndex < skyboxSize * skyboxSize; ++pixelIndex) {
                    defaultTexPixels.push_back(150); //R
                    defaultTexPixels.push_back(50); //G
                    defaultTexPixels.push_back(255); //B
                    defaultTexPixels.push_back(255); //A
                }
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
        //texture creation
        std::vector<const void*> cubeDataPtr = {&skyboxImages[0]->getTexels()[0], &skyboxImages[1]->getTexels()[0], &skyboxImages[2]->getTexels()[0],
                                                &skyboxImages[3]->getTexels()[0], &skyboxImages[4]->getTexels()[0], &skyboxImages[5]->getTexels()[0] };
        auto skyboxTexture = Texture::buildCubeMap(skyboxImages[0]->getWidth(), skyboxImages[0]->getHeight(), skyboxImages[0]->retrieveTextureFormat(), cubeDataPtr);
        std::fill(begin(skyboxImages), end(skyboxImages), std::shared_ptr<Image>(nullptr));

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

        Matrix4<float> projectionMatrix, viewMatrix;
        skyboxRenderer = GenericRendererBuilder::create("skybox", renderTarget, *skyboxShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(sizeof(projectionMatrix), &projectionMatrix) //binding 0
                ->addUniformData(sizeof(viewMatrix), &viewMatrix) //binding 1
                ->addUniformTextureReader(TextureReader::build(skyboxTexture, TextureParam::buildNearest())) //binding 2
                ->build();

        this->isInitialized = true;
    }

    void Skybox::onCameraProjectionUpdate(const Matrix4<float>& projectionMatrix) const {
        skyboxRenderer->updateUniformData(0, &projectionMatrix);
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

    void Skybox::prepareRendering(unsigned int& renderingOrder, const Matrix4<float>& viewMatrix, const Point3<float>& cameraPosition) {
        assert(isInitialized);
        translationMatrix.buildTranslation(cameraPosition.X, cameraPosition.Y + offsetY, cameraPosition.Z);
        Matrix4<float> skyboxViewMatrix = viewMatrix * translationMatrix;
        skyboxRenderer->updateUniformData(1, &skyboxViewMatrix);

        skyboxRenderer->enableRenderer(renderingOrder);
    }

}
