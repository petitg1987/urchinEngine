#include <stdexcept>

#include "Skybox.h"
#include "resources/MediaManager.h"
#include "graphic/render/GenericRendererBuilder.h"
#include "graphic/texture/Texture.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {
    /**
    * @param filenames Filenames of the textures in the following order: X-, X+, Y-, Y+, Z-, Z+
    */
    Skybox::Skybox(const std::vector<std::string>& filenames) :
            isInitialized(false),
            filenames(filenames),
            offsetY(0.0) {
        if (filenames.size() != 6) {
            throw std::invalid_argument("There is no 6 skybox filenames.");
        }

        //create the textures
        skyboxImages = new Image*[6];
        unsigned int skyboxSize = 1;
        for (std::size_t i = 0; i < 6; ++i) {
            if (!filenames[i].empty()) {
                skyboxImages[i] = MediaManager::instance()->getMedia<Image>(filenames[i]);
                skyboxSize = skyboxImages[i]->getWidth();
            }
        }

        for (std::size_t i = 0; i < 6; ++i) {
            if (filenames[i].empty()) {
                std::vector<unsigned char> defaultTexPixels;
                defaultTexPixels.reserve(skyboxSize * skyboxSize * 4);
                for (std::size_t pixelIndex = 0; pixelIndex < skyboxSize * skyboxSize; ++pixelIndex) {
                    defaultTexPixels.push_back(150); //R
                    defaultTexPixels.push_back(50); //G
                    defaultTexPixels.push_back(255); //B
                    defaultTexPixels.push_back(255); //A
                }
                skyboxImages[i] = new Image(skyboxSize, skyboxSize, Image::IMAGE_RGBA, std::move(defaultTexPixels));
            }
        }

        for (std::size_t i = 0; i < 6 - 1; ++i) {
            unsigned int widthSize = skyboxImages[i]->getWidth();
            unsigned int heightSize = skyboxImages[i]->getHeight();
            TextureFormat textureFormat = skyboxImages[0]->retrieveTextureFormat();

            unsigned int nextWidthSize = skyboxImages[i + 1]->getWidth();
            TextureFormat nextTextureFormat = skyboxImages[i + 1]->retrieveTextureFormat();

            if (skyboxImages[i]->getWidth() != skyboxImages[i]->getHeight()) {
                clearSkyboxImages();
                throw std::runtime_error("Skybox image must be a square. Present image size: " + std::to_string(widthSize) + "x" + std::to_string(heightSize));
            } else if (widthSize != nextWidthSize) {
                clearSkyboxImages();
                throw std::runtime_error("All skybox images must have the same size: " + std::to_string(widthSize) + " != " + std::to_string(nextWidthSize));
            } else if (textureFormat != nextTextureFormat) {
                clearSkyboxImages();
                throw std::runtime_error("All skybox images must have the same texture format: " + std::to_string(textureFormat) + " != " + std::to_string(nextTextureFormat));
            }
        }
    }

    Skybox::~Skybox() {
        clearSkyboxImages();
    }

    void Skybox::initialize(std::shared_ptr<RenderTarget> renderTarget) {
        this->renderTarget = std::move(renderTarget);

        //texture creation
        std::vector<const void*> cubeDataPtr = {&skyboxImages[0]->getTexels()[0], &skyboxImages[1]->getTexels()[0], &skyboxImages[2]->getTexels()[0],
                                                 &skyboxImages[3]->getTexels()[0], &skyboxImages[4]->getTexels()[0], &skyboxImages[5]->getTexels()[0] };
        auto skyboxTexture = Texture::buildCubeMap(skyboxImages[0]->getWidth(), skyboxImages[0]->getHeight(), skyboxImages[0]->retrieveTextureFormat(), cubeDataPtr);
        clearSkyboxImages();

        //visual
        skyboxShader = ShaderBuilder().createShader("skybox.vert", "", "skybox.frag");

        mProjectionShaderVar = ShaderVar(skyboxShader, "mProjection");
        mViewShaderVar = ShaderVar(skyboxShader, "mView");

        int diffuseTexUnit = 0;
        ShaderDataSender(true).sendData(ShaderVar(skyboxShader, "diffuseTexture"), diffuseTexUnit); //binding 20

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
            Point3<float>(-SIZE, SIZE, SIZE), Point3<float>(-SIZE, -SIZE, SIZE), Point3<float>(-SIZE, -SIZE, -SIZE),
            Point3<float>(-SIZE, SIZE, SIZE), Point3<float>(-SIZE, -SIZE, -SIZE), Point3<float>(-SIZE, SIZE, -SIZE),
            //x positive:
            Point3<float>(SIZE, SIZE, -SIZE), Point3<float>(SIZE, -SIZE, -SIZE), Point3<float>(SIZE, -SIZE, SIZE),
            Point3<float>(SIZE, SIZE, -SIZE), Point3<float>(SIZE, -SIZE, SIZE), Point3<float>(SIZE, SIZE, SIZE),
            //y negative:
            Point3<float>(-SIZE, -SIZE, SIZE), Point3<float>(SIZE, -SIZE, SIZE), Point3<float>(SIZE, -SIZE, -SIZE),
            Point3<float>(-SIZE, -SIZE, SIZE), Point3<float>(SIZE, -SIZE, -SIZE), Point3<float>(-SIZE, -SIZE, -SIZE),
            //y positive:
            Point3<float>(-SIZE, SIZE, -SIZE), Point3<float>(SIZE, SIZE, -SIZE), Point3<float>(SIZE, SIZE, SIZE),
            Point3<float>(-SIZE, SIZE, -SIZE), Point3<float>(SIZE, SIZE, SIZE), Point3<float>(-SIZE, SIZE, SIZE),
            //z negative:
            Point3<float>(-SIZE, SIZE, -SIZE), Point3<float>(-SIZE, -SIZE, -SIZE), Point3<float>(SIZE, -SIZE, -SIZE),
            Point3<float>(-SIZE, SIZE, -SIZE), Point3<float>(SIZE, -SIZE, -SIZE), Point3<float>(SIZE, SIZE, -SIZE),
            //z positive:
            Point3<float>(SIZE, SIZE, SIZE), Point3<float>(SIZE, -SIZE, SIZE), Point3<float>(-SIZE, -SIZE, SIZE),
            Point3<float>(SIZE, SIZE, SIZE), Point3<float>(-SIZE, -SIZE, SIZE), Point3<float>(-SIZE, SIZE, SIZE)
        };

        skyboxRenderer = std::make_unique<GenericRendererBuilder>(this->renderTarget, skyboxShader, ShapeType::TRIANGLE)
                ->addData(&vertexCoord)
                ->addData(&textureCoord)
                ->addTextureReader(TextureReader::build(skyboxTexture, TextureParam::buildLinear()))
                ->addShaderData(ShaderDataSender(true).sendData(mProjectionShaderVar, Matrix4<float>())) //binding 0
                ->addShaderData(ShaderDataSender(true).sendData(mViewShaderVar, Matrix4<float>())) //binding 1
                ->build();

        this->isInitialized = true;
    }

    void Skybox::clearSkyboxImages() {
        if (skyboxImages != nullptr) {
            for (std::size_t i = 0; i < 6; i++) {
                skyboxImages[i]->release();
            }
            delete[] skyboxImages;
        }
        skyboxImages = nullptr;
    }

    void Skybox::onCameraProjectionUpdate(const Matrix4<float>& projectionMatrix) const {
        skyboxRenderer->updateShaderData(0, ShaderDataSender(true).sendData(mProjectionShaderVar, projectionMatrix));
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

    void Skybox::display(const Matrix4<float>& viewMatrix, const Point3<float>& cameraPosition) {
        assert(isInitialized);
        translationMatrix.buildTranslation(cameraPosition.X, cameraPosition.Y + offsetY, cameraPosition.Z);
        skyboxRenderer->updateShaderData(1, ShaderDataSender(true).sendData(mViewShaderVar, viewMatrix * translationMatrix));

        renderTarget->display(skyboxRenderer);
    }

}
