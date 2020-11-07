#include <GL/glew.h>
#include <stdexcept>
#include <cassert>

#include "graphic/render/GenericRenderer.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    GenericRenderer::GenericRenderer(const GenericRendererBuilder *rendererBuilder) :
            shapeType(rendererBuilder->getShapeType()),
            shapeCount(rendererBuilder->getShapeCount()),
            vertexCoordType(rendererBuilder->getVertexCoordType()),
            vertexCoordDimension(rendererBuilder->getVertexCoordDimension()),
            textureCoordType(rendererBuilder->getTextureCoordType()),
            textureCoordDimension(rendererBuilder->getTextureCoordDimension()),
            transparencyEnabled(rendererBuilder->isTransparencyEnabled()),
            depthTestEnabled(rendererBuilder->isDepthTestEnabled()),
            textures(rendererBuilder->getTextures()),
            bufferIDs(),
            vertexArrayObject(0) {
        glGenBuffers(2, bufferIDs);
        glGenVertexArrays(1, &vertexArrayObject);

        for (const auto &texture : textures) {
            initializeTexture(texture);
        }
        additionalTextures.reserve(2); //estimated memory size

        initializeDisplay(rendererBuilder->getVertexCoord(), rendererBuilder->getTextureCoord());
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

    void GenericRenderer::initializeDisplay(void *vertexCoord, void *textureCoord) {
        glBindVertexArray(vertexArrayObject);

        auto vertexCoordDim = coordDimensionToSize(vertexCoordDimension);
        auto vertexMemorySize = coordTypeToSize(vertexCoordType) * vertexCoordDim * shapeCount * shapeTypeToVertexCount(shapeType);
        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
        glBufferData(GL_ARRAY_BUFFER, vertexMemorySize, vertexCoord, GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
        glVertexAttribPointer(SHADER_VERTEX_POSITION, vertexCoordDim, coordTypeToGlType(vertexCoordType), GL_FALSE, 0, nullptr);

        auto textureCoordDim = coordDimensionToSize(textureCoordDimension);
        auto textureMemorySize = coordTypeToSize(textureCoordType) * textureCoordDim * shapeCount * shapeTypeToVertexCount(shapeType);
        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
        glBufferData(GL_ARRAY_BUFFER, textureMemorySize, textureCoord, GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_TEX_COORD);
        glVertexAttribPointer(SHADER_TEX_COORD, textureCoordDim, coordTypeToGlType(textureCoordType), GL_FALSE, 0, nullptr);
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

    unsigned int GenericRenderer::coordTypeToSize(CoordType coordType) const {
        if (coordType == CoordType::FLOAT) {
            return sizeof(float);
        } else if (coordType == CoordType::INT) {
            return sizeof(int);
        } else if (coordType == CoordType::UNSIGNED_INT) {
            return sizeof(unsigned int);
        }
        throw std::runtime_error("Unknown coordinate type: " + std::to_string(coordType));
    }

    unsigned int GenericRenderer::coordTypeToGlType(CoordType coordType) const {
        if (coordType == CoordType::FLOAT) {
            return GL_FLOAT;
        } else if (coordType == CoordType::INT) {
            return GL_INT;
        } else if (coordType == CoordType::UNSIGNED_INT) {
            return GL_UNSIGNED_INT;
        }
        throw std::runtime_error("Unknown coordinate type: " + std::to_string(coordType));
    }

    unsigned int GenericRenderer::coordDimensionToSize(CoordDimension coordDimension) const {
        if (coordDimension == CoordDimension::_2D) {
            return 2;
        } else if (coordDimension == CoordDimension::_3D) {
            return 3;
        } else if (coordDimension == CoordDimension::_4D) {
            return 4;
        }
        throw std::runtime_error("Unknown coordinate dimension: " + std::to_string(coordDimension));
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
        } else {
            glDisable(GL_BLEND);
        }

        if(depthTestEnabled) {
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
        }

        glBindVertexArray(vertexArrayObject);
        glDrawArrays(shapeTypeToGlType(shapeType), 0, shapeTypeToVertexCount(shapeType) * shapeCount);

        //reset to default values
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }
}
