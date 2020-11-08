#include <GL/glew.h>
#include <stdexcept>

#include "Skybox.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "resources/MediaManager.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {
    /**
    * @param filenames Filenames of the textures in the following order: X-, X+, Y-, Y+, Z-, Z+
    */
    Skybox::Skybox(const std::vector<std::string> &filenames) :
            filenames(filenames),
            textureID(0),
            offsetY(0.0),
            mProjectionLoc(0),
            mViewLoc(0) {
        if (filenames.size() != 6) {
            throw std::invalid_argument("There is no 6 skybox filenames.");
        }

        //create the textures
        texSkybox = new Image*[6];
        unsigned int skyboxSize = 1;
        for (std::size_t i=0; i < 6; ++i) {
            if (!filenames[i].empty()) {
                texSkybox[i] = MediaManager::instance()->getMedia<Image>(filenames[i]);
                skyboxSize = texSkybox[i]->getWidth();
            }
        }

        for (std::size_t i=0; i < 6; ++i) {
            if (filenames[i].empty()) {
                std::vector<unsigned char> defaultTexPixels;
                defaultTexPixels.reserve(skyboxSize * skyboxSize);
                for (std::size_t pixelIndex=0; pixelIndex < skyboxSize * skyboxSize; ++pixelIndex) {
                    defaultTexPixels.push_back(150); //R
                    defaultTexPixels.push_back(50); //G
                    defaultTexPixels.push_back(255); //B
                }
                texSkybox[i] = new Image(skyboxSize, skyboxSize, Image::IMAGE_RGB, std::move(defaultTexPixels));
            }
        }

        for (std::size_t i=0; i < 6 - 1; ++i) {
            unsigned int widthSize = texSkybox[i]->getWidth();
            unsigned int heightSize = texSkybox[i]->getHeight();
            unsigned int nextWidthSize = texSkybox[i + 1]->getWidth();

            if (texSkybox[i]->getWidth() != texSkybox[i]->getHeight()) {
                clearTexSkybox();
                throw std::runtime_error("Skybox image must be a square. Present image size: " + std::to_string(widthSize) + "x" + std::to_string(heightSize));
            } else if (widthSize != nextWidthSize) {
                clearTexSkybox();
                throw std::runtime_error("All skybox images must have the same size: " + std::to_string(widthSize) + " != " + std::to_string(nextWidthSize));
            }
        }

        initialize();
    }

    Skybox::~Skybox() {
        clearTexSkybox();
        glDeleteTextures(1, &textureID);
    }

    void Skybox::initialize() {
        GLenum cubeMapTarget[6] = {
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z
        };
        const float SIZE = 10.0f;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        for (std::size_t i=0; i<6; i++) {
            glTexImage2D(cubeMapTarget[i], 0, texSkybox[i]->retrieveInternalFormat(), texSkybox[i]->getWidth(), texSkybox[i]->getHeight(), 0, texSkybox[i]->retrieveFormat(), GL_UNSIGNED_BYTE, &texSkybox[i]->getTexels()[0]);
        }
        clearTexSkybox();

        //visual
        skyboxShader = ShaderBuilder().createShader("skybox.vert", "", "skybox.frag");

        skyboxShader->bind();
        mProjectionLoc = glGetUniformLocation(skyboxShader->getShaderId(), "mProjection");
        mViewLoc = glGetUniformLocation(skyboxShader->getShaderId(), "mView");
        int diffuseTexSamplerLoc = glGetUniformLocation(skyboxShader->getShaderId(), "diffuseTexture");
        glUniform1i(diffuseTexSamplerLoc, GL_TEXTURE0-GL_TEXTURE0);

        std::vector<float> vertexCoord = {
            //x negative:
            -SIZE, -SIZE, SIZE, -SIZE, SIZE, SIZE, -SIZE, SIZE, -SIZE, -SIZE, -SIZE, -SIZE,
            //x positive:
            SIZE, -SIZE, -SIZE, SIZE, SIZE, -SIZE, SIZE, SIZE, SIZE, SIZE, -SIZE, SIZE,
            //y negative:
            -SIZE, -SIZE, -SIZE, SIZE, -SIZE, -SIZE, SIZE, -SIZE, SIZE, -SIZE, -SIZE, SIZE,
            //y positive:
            -SIZE, SIZE, SIZE,SIZE, SIZE, SIZE, SIZE, SIZE, -SIZE, -SIZE, SIZE, -SIZE,
            //z negative:
            -SIZE, -SIZE, -SIZE, -SIZE, SIZE, -SIZE, SIZE, SIZE, -SIZE, SIZE, -SIZE, -SIZE,
            //z positive:
            SIZE, -SIZE, SIZE, SIZE, SIZE, SIZE, -SIZE, SIZE, SIZE, -SIZE, -SIZE, SIZE,
        };

        std::vector<float> textureCoord = {
            //x negative:
            -SIZE, SIZE, SIZE, -SIZE, -SIZE, SIZE, -SIZE, -SIZE, -SIZE, -SIZE, SIZE, -SIZE,
            //x positive:
            SIZE, SIZE, -SIZE, SIZE, -SIZE, -SIZE, SIZE, -SIZE, SIZE, SIZE, SIZE, SIZE,
            //y negative:
            -SIZE, -SIZE, SIZE, SIZE, -SIZE, SIZE, SIZE, -SIZE, -SIZE, -SIZE, -SIZE, -SIZE,
            //y positive:
            -SIZE, SIZE, -SIZE, SIZE, SIZE, -SIZE, SIZE, SIZE, SIZE, -SIZE, SIZE, SIZE,
            //z negative:
            -SIZE, SIZE, -SIZE, -SIZE, -SIZE, -SIZE, SIZE, -SIZE, -SIZE, SIZE, SIZE, -SIZE,
            //z positive:
            SIZE, SIZE, SIZE, SIZE, -SIZE, SIZE, -SIZE, -SIZE, SIZE, -SIZE, SIZE, SIZE,
        };

        skyboxRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::RECTANGLE)
                ->shapeCount(6)
                ->vertexData(CoordType::FLOAT, CoordDimension::_3D, &vertexCoord[0])
                ->textureData(CoordType::FLOAT, CoordDimension::_3D, &textureCoord[0])
                ->addTexture(Texture::build(textureID, Texture::Type::CUBE_MAP, TextureParam::buildLinear()))
                ->build();
    }

    void Skybox::clearTexSkybox() {
        if (texSkybox != nullptr) {
            for (std::size_t i = 0; i < 6; i++) {
                texSkybox[i]->release();
            }
            delete[] texSkybox;
        }
        texSkybox = nullptr;
    }

    void Skybox::onCameraProjectionUpdate(const Matrix4<float> &projectionMatrix) const {
        skyboxShader->bind();
        glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float*)projectionMatrix);
    }

    float Skybox::getOffsetY() const {
        return offsetY;
    }

    void Skybox::setOffsetY(float offsetY) {
        this->offsetY = offsetY;
    }

    const std::vector<std::string> &Skybox::getFilenames() const {
        return filenames;
    }

    void Skybox::display(const Matrix4<float> &viewMatrix, const Point3<float> &cameraPosition) {
        skyboxShader->bind();

        translationMatrix.buildTranslation(cameraPosition.X, cameraPosition.Y + offsetY, cameraPosition.Z);
        glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, (const float*)(viewMatrix * translationMatrix));

        skyboxRenderer->draw();
    }

}
