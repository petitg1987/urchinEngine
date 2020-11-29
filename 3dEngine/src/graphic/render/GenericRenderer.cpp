#include <GL/glew.h>
#include <stdexcept>
#include <cassert>

#include "graphic/render/GenericRenderer.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    //static
    const unsigned int GenericRenderer::PRIMITIVE_RESTART_INDEX_VALUE = std::numeric_limits<unsigned int>::max();

    GenericRenderer::GenericRenderer(const GenericRendererBuilder* rendererBuilder) :
            shapeType(rendererBuilder->getShapeType()),
            data(rendererBuilder->getData()),
            indices(rendererBuilder->getIndices()),
            transparencyEnabled(rendererBuilder->isTransparencyEnabled()),
            depthTestEnabled(rendererBuilder->isDepthTestEnabled()),
            cullFaceEnabled(rendererBuilder->isCullFaceEnabled()),
            polygonMode(rendererBuilder->getPolygonMode()),
            outlineSize(rendererBuilder->getOutlineSize()),
            textureReaders(rendererBuilder->getTextures()),
            bNeedRenderTextures(true),
            vertexArrayObject(0) {

        verticesCount = computeVerticesCount();

        glGenVertexArrays(1, &vertexArrayObject);
        initializeDisplay();

        for (const auto& textureReader : textureReaders) {
            initializeTexture(textureReader);
        }
        additionalTextureReaders.reserve(2); //estimated memory size
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
            for (const auto& dataValue : data) {
                assert(dataValue.dataCount == countResult);
            }
        #endif
        return countResult;
    }

    void GenericRenderer::initializeTexture(const TextureReader& textureReader) const {
        unsigned int textureType = textureReader.getTexture()->getGlTextureType();
        glBindTexture(textureType, textureReader.getTexture()->getTextureId());

        unsigned int readMode = textureReader.getParam().getGlReadMode();
        glTexParameteri(textureType, GL_TEXTURE_WRAP_S, readMode);
        glTexParameteri(textureType, GL_TEXTURE_WRAP_T, readMode);
        if (textureType == GL_TEXTURE_CUBE_MAP) {
            glTexParameteri(textureType, GL_TEXTURE_WRAP_R, readMode);
        }

        glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, textureReader.getParam().getGlReadQuality(textureReader.getTexture()->hasMipmap()));
        glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, textureReader.getParam().getGlReadQuality(false));

        if (textureReader.getParam().needAnisotropy() && GLEW_EXT_texture_filter_anisotropic) {
            float maxAnisotropy = 1.0f;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
            glTexParameterf(textureType, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
        }
    }

    void GenericRenderer::initializeDisplay() {
        glBindVertexArray(vertexArrayObject);

        for(std::size_t dataIndex = 0; dataIndex < data.size(); dataIndex++) {
            unsigned int bufferId = 0;
            glGenBuffers(1, &bufferId);
            bufferIds.push_back(bufferId);

            sendData(dataIndex, false);
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

    void GenericRenderer::sendData(std::size_t dataIndex, bool update) {
        Data& dataValue = data[dataIndex];

        auto dataDim = dataDimensionToSize(dataValue.dataDimension);
        auto dataMemorySize = dataTypeToSize(dataValue.dataType) * dataDim * dataValue.dataCount;

        glBindVertexArray(vertexArrayObject);
        glBindBuffer(GL_ARRAY_BUFFER, bufferIds[dataIndex]);
        glBufferData(GL_ARRAY_BUFFER, dataMemorySize, dataValue.ptr, update ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        glEnableVertexAttribArray(dataIndex);
        glVertexAttribPointer(dataIndex, dataDimensionToSize(dataValue.dataDimension), dataTypeToGlType(dataValue.dataType), GL_FALSE, 0, nullptr);

        dataValue.ptr = nullptr; //reset pointer because no guaranteed pointer is still valid after initialization
    }

    unsigned int GenericRenderer::shapeTypeToGlType(ShapeType shapeType) const {
        if (shapeType == ShapeType::TRIANGLE) {
            return GL_TRIANGLES;
        } else if (shapeType == ShapeType::TRIANGLE_STRIP) {
            return GL_TRIANGLE_STRIP;
        } else if (shapeType == ShapeType::LINE) {
            return GL_LINES;
        } else if (shapeType == ShapeType::LINE_STRIP) {
            return GL_LINE_STRIP;
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

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Point2<float>>* dataPtr) {
        assert(data.size() > dataIndex);
        GenericRenderer::Data dataValue{DataType::FLOAT, DataDimension::TWO_DIMENSION, &(*dataPtr)[0], (unsigned int)dataPtr->size()};
        data[dataIndex] = dataValue;

        sendData(dataIndex, true);
    }

    void GenericRenderer::updateData(std::size_t dataIndex, const std::vector<Point3<float>>* dataPtr) {
        assert(data.size() > dataIndex);
        GenericRenderer::Data dataValue{DataType::FLOAT, DataDimension::THREE_DIMENSION, &(*dataPtr)[0], (unsigned int)dataPtr->size()};
        data[dataIndex] = dataValue;

        sendData(dataIndex, true);
    }

    void GenericRenderer::updateData(std::size_t pointsCoordIndex, const std::vector<Vector3<float>>* dataPtr) {
        updateData(pointsCoordIndex, reinterpret_cast<const std::vector<Point3<float>>*>(dataPtr));
    }

    void GenericRenderer::updateTexture(std::size_t textureIndex, const TextureReader& texture) {
        assert(textureReaders.size() > textureIndex);

        initializeTexture(texture);
        textureReaders[textureIndex] = texture;
    }

    /**
     * @return Texture unit
     */
    unsigned int GenericRenderer::addAdditionalTexture(const TextureReader& texture) {
        initializeTexture(texture);

        additionalTextureReaders.push_back(texture);
        return textureReaders.size() + additionalTextureReaders.size() - 1;
    }

    void GenericRenderer::clearAdditionalTextures() {
        additionalTextureReaders.clear();
    }

    void GenericRenderer::renderTextures(bool bNeedRenderTextures) {
        this->bNeedRenderTextures = bNeedRenderTextures;
    }

    void GenericRenderer::render() const {
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        if(bNeedRenderTextures) {
            unsigned int textureUnit = 0;
            for (const auto& textureReader : textureReaders) {
                glActiveTexture(GL_TEXTURE0 + textureUnit++);
                glBindTexture(textureReader.getTexture()->getGlTextureType(), textureReader.getTexture()->getTextureId());
            }
            for (const auto& textureReader : additionalTextureReaders) {
                glActiveTexture(GL_TEXTURE0 + textureUnit++);
                glBindTexture(textureReader.getTexture()->getGlTextureType(), textureReader.getTexture()->getTextureId());
            }
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
            if(indices.hasPrimitiveRestartIndex) {
                glEnable(GL_PRIMITIVE_RESTART);
                glPrimitiveRestartIndex(GenericRenderer::PRIMITIVE_RESTART_INDEX_VALUE);
            }

            glDrawElements(shapeTypeToGlType(shapeType), verticesCount, GL_UNSIGNED_INT, nullptr);

            if(indices.hasPrimitiveRestartIndex) {
                glDisable(GL_PRIMITIVE_RESTART);
            }
        } else {
            glDrawArrays(shapeTypeToGlType(shapeType), 0, verticesCount);
        }

        resetRenderDefaultValues();
    }

    void GenericRenderer::resetRenderDefaultValues() const {
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
