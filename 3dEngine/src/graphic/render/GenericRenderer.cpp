#include <GL/glew.h>
#include <stdexcept>
#include <cassert>

#include "graphic/render/GenericRenderer.h"
#include "graphic/render/GenericRendererBuilder.h"
#include "graphic/texture/TextureManager.h"

namespace urchin {

    GenericRenderer::GenericRenderer(const GenericRendererBuilder *rendererBuilder) :
            shapeType(rendererBuilder->getShapeType()),
            pointsCoords(rendererBuilder->getPointsCoords()),
            transparencyEnabled(rendererBuilder->isTransparencyEnabled()),
            depthTestEnabled(rendererBuilder->isDepthTestEnabled()),
            cullFaceEnabled(rendererBuilder->isCullFaceEnabled()),
            polygonMode(rendererBuilder->getPolygonMode()),
            outlineSize(rendererBuilder->getOutlineSize()),
            textures(rendererBuilder->getTextures()),
            vertexArrayObject(0) {

        assert(!pointsCoords.empty());
        pointsCount = pointsCoords[0].pointsCount;
        for(const auto pointCoord : pointsCoords) {
            assert(pointCoord.pointsCount == pointsCount);
        }

        glGenVertexArrays(1, &vertexArrayObject);
        initializeDisplay();

        for (const auto &texture : textures) {
            initializeTexture(texture);
        }
        additionalTextures.reserve(2); //estimated memory size
    }

    GenericRenderer::~GenericRenderer() {
        if (vertexArrayObject != 0) {
            glDeleteVertexArrays(1, &vertexArrayObject);
        }

        for(unsigned int bufferId : bufferIds) {
            glDeleteBuffers(1, &bufferId);
        }
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

        glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, texture.getParam().getGlReadQualityMinifyingFilter());
        glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, texture.getParam().getGlReadQualityMagnificationFilter());

        if (texture.getParam().needAnisotropy() && GLEW_EXT_texture_filter_anisotropic) {
            glTexParameterf(textureType, GL_TEXTURE_MAX_ANISOTROPY_EXT, TextureManager::instance()->getAnisotropy());
        }
    }

    void GenericRenderer::initializeDisplay() {
        glBindVertexArray(vertexArrayObject);

        unsigned int pointsCoordUnit = 0;
        for(auto &pointsCoord : pointsCoords) {
            unsigned int bufferId = 0;
            glGenBuffers(1, &bufferId);
            bufferIds.push_back(bufferId);

            auto vertexCoordDim = coordDimensionToSize(pointsCoord.coordDimension);
            auto vertexMemorySize = coordTypeToSize(pointsCoord.coordType) * vertexCoordDim * pointsCoord.pointsCount;
            glBindBuffer(GL_ARRAY_BUFFER, bufferId);
            glBufferData(GL_ARRAY_BUFFER, vertexMemorySize, pointsCoord.points, GL_STATIC_DRAW);
            glEnableVertexAttribArray(pointsCoordUnit);
            glVertexAttribPointer(pointsCoordUnit, vertexCoordDim, coordTypeToGlType(pointsCoord.coordType), GL_FALSE, 0, nullptr);

            pointsCoord.points = nullptr; //reset pointer because no guaranteed pointer is still valid after initialization
            pointsCoordUnit++;
        }
    }

    unsigned int GenericRenderer::shapeTypeToGlType(ShapeType shapeType) const {
        if (shapeType == ShapeType::RECTANGLE) {
            return GL_QUADS;
        } else if (shapeType == ShapeType::LINE) {
            return GL_LINES;
        } else if (shapeType == ShapeType::LINE_STRIP) {
            return GL_LINE_STRIP;
        } else if (shapeType == ShapeType::TRIANGLE) {
            return GL_TRIANGLES;
        } else if (shapeType == ShapeType::TRIANGLE_STRIP) {
            return GL_TRIANGLE_STRIP;
        } else if (shapeType == ShapeType::POINT) {
            return GL_POINTS;
        }
        throw std::runtime_error("Unknown shape type: " + std::to_string(shapeType));
    }

    unsigned int GenericRenderer::coordTypeToSize(CoordType coordType) const {
        if (coordType == CoordType::FLOAT) {
            return sizeof(float);
        }
        throw std::runtime_error("Unknown coordinate type: " + std::to_string(coordType));
    }

    unsigned int GenericRenderer::coordTypeToGlType(CoordType coordType) const {
        if (coordType == CoordType::FLOAT) {
            return GL_FLOAT;
        }
        throw std::runtime_error("Unknown coordinate type: " + std::to_string(coordType));
    }

    unsigned int GenericRenderer::coordDimensionToSize(CoordDimension coordDimension) const {
        if (coordDimension == CoordDimension::TWO_DIMENSION) {
            return 2;
        } else if (coordDimension == CoordDimension::THREE_DIMENSION) {
            return 3;
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

        if(!depthTestEnabled) {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
        }

        if(!cullFaceEnabled) {
            glDisable(GL_CULL_FACE);
        }

        if(polygonMode == PolygonMode::WIREFRAME) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        if (shapeType == ShapeType::POINT) {
            glPointSize(outlineSize);
        } else if (shapeType == ShapeType::LINE || shapeType == ShapeType::LINE_STRIP || polygonMode == PolygonMode::WIREFRAME) {
            glLineWidth(outlineSize);
        }

        glBindVertexArray(vertexArrayObject);
        glDrawArrays(shapeTypeToGlType(shapeType), 0, pointsCount);

        resetDrawDefaultValues();
    }

    void GenericRenderer::resetDrawDefaultValues() const {
        if(transparencyEnabled) {
            glDisable(GL_BLEND);
        }

        if(!depthTestEnabled) {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
        }

        if(!cullFaceEnabled) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
        }

        if(polygonMode != PolygonMode::FILL) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glLineWidth(1.0f);
        glPointSize(1.0f);
    }
}
