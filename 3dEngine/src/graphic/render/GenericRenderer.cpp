#include <GL/glew.h>
#include <stdexcept>
#include <cassert>

#include "graphic/render/GenericRenderer.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    GenericRenderer::GenericRenderer(const GenericRendererBuilder *rendererBuilder) :
            shapeType(rendererBuilder->getShapeType()),
            shapeCount(rendererBuilder->getShapeCount()),
            dimension(rendererBuilder->getDimension()),
            vertexDataType(rendererBuilder->getVertexDataType()),
            vertexCoord(rendererBuilder->getVertexCoord()),
            textureDataType(rendererBuilder->getTextureDataType()),
            textureCoord(rendererBuilder->getTextureCoord()),
            transparencyEnabled(rendererBuilder->isTransparencyEnabled()),
            textures(rendererBuilder->getTextures()),
            bufferIDs(),
            vertexArrayObject(0) {
        glGenBuffers(2, bufferIDs);
        glGenVertexArrays(1, &vertexArrayObject);

        for (const auto &texture : textures) {
            initializeTexture(texture);
        }
        additionalTextures.reserve(2); //estimated memory size

        initializeDisplay(rendererBuilder->isDeleteVertexCoord(), rendererBuilder->isDeleteTextureCoord());
    }

    GenericRenderer::~GenericRenderer() {
        if (vertexArrayObject!=0) {
            glDeleteVertexArrays(1, &vertexArrayObject);
        }

        glDeleteBuffers(2, bufferIDs);
    }

    void GenericRenderer::initializeTexture(Texture texture) const {
        unsigned int textureType = texture.getGlType();
        glBindTexture(textureType, texture.getId());

        unsigned int readMode = texture.getParam().getGlReadMode();
        glTexParameteri(textureType, GL_TEXTURE_WRAP_S, readMode);
        glTexParameteri(textureType, GL_TEXTURE_WRAP_T, readMode);
        if (textureType == GL_TEXTURE_CUBE_MAP) {
            glTexParameteri(textureType, GL_TEXTURE_WRAP_R, readMode);
        }

        unsigned int readQuality = texture.getParam().getGlReadQuality();
        glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, readQuality);
        glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, readQuality);
    }

    void GenericRenderer::initializeDisplay(bool deleteVertexCoord, bool deleteTextureCoord) {
        glBindVertexArray(vertexArrayObject);

        const unsigned int vertexSize = dataTypeToSize(vertexDataType) * dimension * shapeCount * shapeTypeToVertexCount(shapeType);
        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
        glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexCoord, GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
        glVertexAttribPointer(SHADER_VERTEX_POSITION, dimension, dataTypeToGlType(vertexDataType), GL_FALSE, 0, nullptr);
        if (deleteVertexCoord) {
            if (vertexDataType == CoordDataType::FLOAT) {
                delete[] static_cast<float *>(vertexCoord);
            } else if (vertexDataType == CoordDataType::INT) {
                delete[] static_cast<int *>(vertexCoord);
            } else if (vertexDataType == CoordDataType::UNSIGNED_INT) {
                delete[] static_cast<unsigned int *>(vertexCoord);
            } else {
                throw std::runtime_error("Unknown vertex data type: " + std::to_string(vertexDataType));
            }
            vertexCoord = nullptr;
        }

        const unsigned int textureSize = dataTypeToSize(textureDataType) * dimension * shapeCount * shapeTypeToVertexCount(shapeType);
        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
        glBufferData(GL_ARRAY_BUFFER, textureSize, textureCoord, GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_TEX_COORD);
        glVertexAttribPointer(SHADER_TEX_COORD, dimension, dataTypeToGlType(textureDataType), GL_FALSE, 0, nullptr);
        if (deleteTextureCoord) {
            if (textureDataType == CoordDataType::FLOAT) {
                delete[] static_cast<float *>(textureCoord);
            } else if (textureDataType == CoordDataType::INT) {
                delete[] static_cast<int *>(textureCoord);
            } else if (textureDataType == CoordDataType::UNSIGNED_INT) {
                delete[] static_cast<unsigned int *>(textureCoord);
            } else {
                throw std::runtime_error("Unknown texture data type: " + std::to_string(textureDataType));
            }
            textureCoord = nullptr;
        }
    }

    unsigned int GenericRenderer::shapeTypeToVertexCount(ShapeType shapeType) const {
        if (shapeType == ShapeType::RECTANGLE) {
            return 4;
        }else if(shapeType == ShapeType::LINE) {
            return 2;
        }
        throw std::runtime_error("Unknown shape type: " + std::to_string(shapeType));
    }

    unsigned int GenericRenderer::shapeTypeToGlType(ShapeType shapeType) const {
        if (shapeType == ShapeType::RECTANGLE) {
            return GL_QUADS;
        }else if(shapeType == ShapeType::LINE) {
            return GL_LINES;
        }
        throw std::runtime_error("Unknown shape type: " + std::to_string(shapeType));
    }

    unsigned int GenericRenderer::dataTypeToSize(CoordDataType dataType) const {
        if (dataType == CoordDataType::FLOAT) {
            return sizeof(float);
        } else if (dataType == CoordDataType::INT) {
            return sizeof(int);
        } else if (dataType == CoordDataType::UNSIGNED_INT) {
            return sizeof(unsigned int);
        }
        throw std::runtime_error("Unknown data type: " + std::to_string(dataType));
    }

    unsigned int GenericRenderer::dataTypeToGlType(CoordDataType dataType) const {
        if (dataType == CoordDataType::FLOAT) {
            return GL_FLOAT;
        } else if (dataType == CoordDataType::INT) {
            return GL_INT;
        } else if (dataType == CoordDataType::UNSIGNED_INT) {
            return GL_UNSIGNED_INT;
        }
        throw std::runtime_error("Unknown data type: " + std::to_string(dataType));
    }

    void GenericRenderer::updateTexture(std::size_t textureUnit, Texture texture) {
        initializeTexture(texture);

        assert(textures.size() > textureUnit);
        textures[textureUnit] = texture;
    }

    /**
     * @return Texture unit
     */
    unsigned int GenericRenderer::addAdditionalTexture(Texture texture) {
        initializeTexture(texture);

        additionalTextures.push_back(texture);
        return textures.size() + additionalTextures.size() - 1;
    }

    void GenericRenderer::clearAdditionalTextures() {
        additionalTextures.clear();
    }

    void GenericRenderer::draw() const {
        if(shapeCount == 0) {
            return;
        }

        unsigned int textureUnit = 0;
        for (const auto &texture : textures) {
            glActiveTexture(GL_TEXTURE0 + textureUnit++);
            glBindTexture(texture.getGlType(), texture.getId());
        }
        for (const auto &additionalTexture : additionalTextures) {
            glActiveTexture(GL_TEXTURE0 + textureUnit++);
            glBindTexture(additionalTexture.getGlType(), additionalTexture.getId());
        }

        if(transparencyEnabled) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        glBindVertexArray(vertexArrayObject);
        glDrawArrays(shapeTypeToGlType(shapeType), 0, shapeTypeToVertexCount(shapeType) * shapeCount);

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);

        if(transparencyEnabled) {
            glDisable(GL_BLEND);
        }
    }
}
