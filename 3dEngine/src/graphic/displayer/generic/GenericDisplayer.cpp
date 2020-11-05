#include <GL/glew.h>
#include <stdexcept>
#include <cassert>

#include "graphic/displayer/generic/GenericDisplayer.h"
#include "graphic/displayer/generic/GenericDisplayerBuilder.h"

namespace urchin {

    GenericDisplayer::GenericDisplayer(const GenericDisplayerBuilder *displayerBuilder) :
            shapeType(displayerBuilder->getShapeType()),
            shapeCount(displayerBuilder->getShapeCount()),
            dimension(displayerBuilder->getDimension()),
            vertexDataType(displayerBuilder->getVertexDataType()),
            vertexCoord(displayerBuilder->getVertexCoord()),
            textureDataType(displayerBuilder->getTextureDataType()),
            textureCoord(displayerBuilder->getTextureCoord()),
            textures(displayerBuilder->getTextures()),
            bufferIDs(),
            vertexArrayObject(0) {
        glGenBuffers(2, bufferIDs);
        glGenVertexArrays(1, &vertexArrayObject);

        initializeDisplay(displayerBuilder->isDeleteVertexCoord(), displayerBuilder->isDeleteTextureCoord());
    }

    GenericDisplayer::~GenericDisplayer() {
        if (vertexArrayObject!=0) {
            glDeleteVertexArrays(1, &vertexArrayObject);
        }

        glDeleteBuffers(2, bufferIDs);
    }

    void GenericDisplayer::initializeDisplay(bool deleteVertexCoord, bool deleteTextureCoord) {
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

    unsigned int GenericDisplayer::shapeTypeToVertexCount(ShapeType shapeType) const {
        if (shapeType == ShapeType::RECTANGLE) {
            return 4;
        }else if(shapeType == ShapeType::LINE) {
            return 2;
        }

        throw std::runtime_error("Unknown shape type: " + std::to_string(shapeType));
    }

    unsigned int GenericDisplayer::shapeTypeToGlType(ShapeType shapeType) const {
        if (shapeType == ShapeType::RECTANGLE) {
            return GL_QUADS;
        }else if(shapeType == ShapeType::LINE) {
            return GL_LINES;
        }

        throw std::runtime_error("Unknown shape type: " + std::to_string(shapeType));
    }

    unsigned int GenericDisplayer::dataTypeToSize(CoordDataType dataType) const {
        if (dataType == CoordDataType::FLOAT) {
            return sizeof(float);
        } else if (dataType == CoordDataType::INT) {
            return sizeof(int);
        } else if (dataType == CoordDataType::UNSIGNED_INT) {
            return sizeof(unsigned int);
        }

        throw std::runtime_error("Unknown data type: " + std::to_string(dataType));
    }

    unsigned int GenericDisplayer::dataTypeToGlType(CoordDataType dataType) const {
        if (dataType == CoordDataType::FLOAT) {
            return GL_FLOAT;
        } else if (dataType == CoordDataType::INT) {
            return GL_INT;
        } else if (dataType == CoordDataType::UNSIGNED_INT) {
            return GL_UNSIGNED_INT;
        }

        throw std::runtime_error("Unknown data type: " + std::to_string(dataType));
    }

    void GenericDisplayer::updateTexture(std::size_t index, Texture texture) {
        assert(textures.size() > index);
        textures[index] = texture;
    }

    void GenericDisplayer::display() const {
        if(shapeCount == 0) {
            return;
        }

        bindTextures();

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        glBindVertexArray(vertexArrayObject);
        glDrawArrays(shapeTypeToGlType(shapeType), 0, shapeTypeToVertexCount(shapeType) * shapeCount);

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }

    void GenericDisplayer::bindTextures() const {
        for (std::size_t i = 0; i < textures.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i);

            if (textures[i].getType() == Texture::SIMPLE) {
                glBindTexture(GL_TEXTURE_2D, textures[i].getId());
            } else if (textures[i].getType() == Texture::ARRAY) {
                glBindTexture(GL_TEXTURE_2D_ARRAY, textures[i].getId());
            } else if (textures[i].getType() == Texture::CUBE_MAP) {
                glBindTexture(GL_TEXTURE_CUBE_MAP, textures[i].getId());
            }
        }
    }

}
