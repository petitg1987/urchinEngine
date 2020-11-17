#include <GL/glew.h>
#include <stdexcept>
#include <cassert>

#include "graphic/render/GenericRenderer.h"
#include "graphic/render/GenericRendererBuilder.h"
#include "graphic/texture/TextureManager.h"

namespace urchin {

    GenericRenderer::GenericRenderer(const GenericRendererBuilder *rendererBuilder) :
            shapeType(rendererBuilder->getShapeType()),
            data(rendererBuilder->getData()),
            indices(rendererBuilder->getIndices()),
            transparencyEnabled(rendererBuilder->isTransparencyEnabled()),
            depthTestEnabled(rendererBuilder->isDepthTestEnabled()),
            cullFaceEnabled(rendererBuilder->isCullFaceEnabled()),
            polygonMode(rendererBuilder->getPolygonMode()),
            outlineSize(rendererBuilder->getOutlineSize()),
            textures(rendererBuilder->getTextures()),
            vertexArrayObject(0) {

        verticesCount = computeVerticesCount();

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

    unsigned int GenericRenderer::computeVerticesCount() const {
        assert(!data.empty());

        if(indices.indicesCount > 0) {
            return indices.indicesCount;
        }

        unsigned int countResult = data[0].dataCount;
        #ifndef NDEBUG
            for (const auto &dataValue : data) {
                assert(dataValue.dataCount == countResult);
            }
        #endif
        return countResult;
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

        unsigned int pointsCoordIndex = 0;
        for(auto &dataValue : data) {
            unsigned int bufferId = 0;
            glGenBuffers(1, &bufferId);
            bufferIds.push_back(bufferId);

            sendPointsCoord(pointsCoordIndex, false);
            glEnableVertexAttribArray(pointsCoordIndex);
            glVertexAttribPointer(pointsCoordIndex, dataDimensionToSize(dataValue.dataDimension), dataTypeToGlType(dataValue.dataType), GL_FALSE, 0, nullptr);

            dataValue.ptr = nullptr; //reset pointer because no guaranteed pointer is still valid after initialization
            pointsCoordIndex++;
        }

        if(indices.ptr) {
            unsigned int bufferId = 0;
            glGenBuffers(1, &bufferId);
            bufferIds.push_back(bufferId);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.indicesCount * sizeof(unsigned int), indices.ptr, GL_STATIC_DRAW);

            indices.ptr = nullptr; //reset pointer because no guaranteed pointer is still valid after initialization
        }
    }

    void GenericRenderer::sendPointsCoord(std::size_t pointsCoordIndex, bool update) const {
        const Data &dataValue = data[pointsCoordIndex];

        auto vertexCoordDim = dataDimensionToSize(dataValue.dataDimension);
        auto vertexMemorySize = dataTypeToSize(dataValue.dataType) * vertexCoordDim * dataValue.dataCount;

        glBindBuffer(GL_ARRAY_BUFFER, bufferIds[pointsCoordIndex]);
        glBufferData(GL_ARRAY_BUFFER, vertexMemorySize, dataValue.ptr, update ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
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

    unsigned int GenericRenderer::dataTypeToSize(DataType dataType) const {
        if (dataType == DataType::FLOAT) {
            return sizeof(float);
        }
        throw std::runtime_error("Unknown data type: " + std::to_string(dataType));
    }

    unsigned int GenericRenderer::dataTypeToGlType(DataType dataType) const {
        if (dataType == DataType::FLOAT) {
            return GL_FLOAT;
        }
        throw std::runtime_error("Unknown data type: " + std::to_string(dataType));
    }

    unsigned int GenericRenderer::dataDimensionToSize(DataDimension dataDimension) const {
        if (dataDimension == DataDimension::TWO_DIMENSION) {
            return 2;
        } else if (dataDimension == DataDimension::THREE_DIMENSION) {
            return 3;
        }
        throw std::runtime_error("Unknown data dimension: " + std::to_string(dataDimension));
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Point3<float>> *dataPtr) {
        assert(data.size() > dataIndex);

        GenericRenderer::Data dataValue{};
        dataValue.dataType = DataType::FLOAT;
        dataValue.dataDimension = DataDimension::THREE_DIMENSION;
        dataValue.ptr = &(*dataPtr)[0];
        dataValue.dataCount = dataPtr->size();
        data[dataIndex] = dataValue;

        sendPointsCoord(dataIndex, true);
    }

    void GenericRenderer::updateData(std::size_t pointsCoordIndex, const std::vector<Vector3<float>> *dataPtr) {
        updateData(pointsCoordIndex, reinterpret_cast<const std::vector<Point3<float>> *>(dataPtr));
    }

    void GenericRenderer::updateTexture(std::size_t textureIndex, Texture texture) {
        assert(textures.size() > textureIndex);

        initializeTexture(texture);
        textures[textureIndex] = texture;
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
        if(indices.indicesCount > 0) {
            glDrawElements(shapeTypeToGlType(shapeType), verticesCount, GL_UNSIGNED_INT, nullptr);
        } else {
            glDrawArrays(shapeTypeToGlType(shapeType), 0, verticesCount);
        }

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
