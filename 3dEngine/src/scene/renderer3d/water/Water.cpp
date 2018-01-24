#include <GL/glew.h>

#include "Water.h"
#include "resources/MediaManager.h"
#include "utils/shader/ShaderManager.h"

#define DEFAULT_WATER_COLOR Vector3<float>(0.08f, 0.22f, 0.29f)
#define DEFAULT_NORMAL_TEXTURE ""

namespace urchin
{

    Water::Water(const Point3<float> &centerPosition, float xSize, float zSize) :
            sumTimeStep(0.0f),
            centerPosition(centerPosition),
            xSize(xSize),
            zSize(zSize),
            normalTexture(nullptr),
            sRepeat(1.0f),
            tRepeat(1.0f)
    {
        glGenBuffers(2, bufferIDs);
        glGenVertexArrays(1, &vertexArrayObject);

        waterShader = ShaderManager::instance()->createProgram("water.vert", "water.frag");
        ShaderManager::instance()->bind(waterShader);

        mProjectionLoc = glGetUniformLocation(waterShader, "mProjection");
        mViewLoc = glGetUniformLocation(waterShader, "mView");
        sumTimeStepLoc = glGetUniformLocation(waterShader, "sumTimeStep");

        waterColorLoc = glGetUniformLocation(waterShader, "waterColor");

        int normalTexLoc = glGetUniformLocation(waterShader, "normalTex");
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(normalTexLoc, 0);

        generateVertex();
        setWaterColor(DEFAULT_WATER_COLOR);
        setNormalTexture(DEFAULT_NORMAL_TEXTURE);
    }

    Water::~Water()
    {
        glDeleteVertexArrays(1, &vertexArrayObject);
        glDeleteBuffers(2, bufferIDs);

        normalTexture->release();

        ShaderManager::instance()->removeProgram(waterShader);
    }

    void Water::generateVertex()
    {
        glBindVertexArray(vertexArrayObject);

        std::vector<Point3<float>> vertices = {
                Point3<float>(-xSize/2.0f, 0.0f, -zSize/2.0f) + centerPosition,
                Point3<float>(xSize/2.0f, 0.0f, -zSize/2.0f) + centerPosition,
                Point3<float>(xSize/2.0f, 0.0f, zSize/2.0f) + centerPosition,
                Point3<float>(-xSize/2.0f, 0.0f, zSize/2.0f) + centerPosition};

        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float)*3, &vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
        glVertexAttribPointer(SHADER_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        std::vector<Point2<float>> texCoord = {
                Point2<float>(0.0f*sRepeat, 0.0f*tRepeat),
                Point2<float>(1.0f*sRepeat, 0.0f*tRepeat),
                Point2<float>(1.0f*sRepeat, 1.0f*tRepeat),
                Point2<float>(0.0f*sRepeat, 1.0f*tRepeat)};

        glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
        glBufferData(GL_ARRAY_BUFFER, texCoord.size()*sizeof(float)*2, &texCoord[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(SHADER_TEX_COORD);
        glVertexAttribPointer(SHADER_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    const Point3<float> &Water::getCenterPosition() const
    {
        return centerPosition;
    }

    float Water::getXSize() const
    {
        return xSize;
    }

    float Water::getZSize() const
    {
        return zSize;
    }

    void Water::setWaterColor(const Vector3<float> &waterColor)
    {
        this->waterColor = waterColor;

        ShaderManager::instance()->bind(waterShader);
        glUniform3fv(waterColorLoc, 1, (const float*) waterColor);
    }

    const Vector3<float> Water::getWaterColor() const
    {
        return waterColor;
    }

    void Water::setNormalTexture(const std::string &normalFilename)
    {
        if(normalTexture!=nullptr)
        {
            normalTexture->release();
        }

        if(normalFilename.empty())
        {
            normalTexture = new Image(1, 1, Image::IMAGE_RGB, std::vector<unsigned char>({0, 255, 0}));
        }else
        {
            normalTexture = MediaManager::instance()->getMedia<Image>(normalFilename, nullptr);
            if(normalTexture->getImageFormat() != Image::IMAGE_RGB)
            {
                normalTexture->release();
                throw std::runtime_error("Water normal texture must have 3 components (RGB). Components: " + std::to_string(normalTexture->retrieveComponentsCount()));
            }
        }

        normalTexture->toTexture(true, true, true);
    }

    const Image *Water::getNormalTexture() const
    {
        return normalTexture;
    }

    void Water::setSRepeat(float sRepeat)
    {
        this->sRepeat = sRepeat;

        generateVertex();
    }

    float Water::getSRepeat() const
    {
        return sRepeat;
    }

    void Water::setTRepeat(float tRepeat)
    {
        this->tRepeat  = tRepeat;

        generateVertex();
    }

    float Water::getTRepeat() const
    {
        return tRepeat;
    }

    void Water::onCameraProjectionUpdate(const Matrix4<float> &projectionMatrix)
    {
        this->projectionMatrix = projectionMatrix;

        ShaderManager::instance()->bind(waterShader);
        glUniformMatrix4fv(mProjectionLoc, 1, GL_FALSE, (const float*)projectionMatrix);
    }

    void Water::display(const Camera *camera, float invFrameRate)
    {
        ShaderManager::instance()->bind(waterShader);

        glUniformMatrix4fv(mViewLoc, 1, GL_FALSE, (const float*)camera->getViewMatrix());

        sumTimeStep += invFrameRate;
        glUniform1f(sumTimeStepLoc, sumTimeStep);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, normalTexture->getTextureID());

        glBindVertexArray(vertexArrayObject);
        glDrawArrays(GL_QUADS, 0, 4);
    }
}
