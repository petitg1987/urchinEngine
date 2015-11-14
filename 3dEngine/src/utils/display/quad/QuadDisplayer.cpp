#include <GL/gl.h>
#include <stdexcept>

#include "utils/display/quad/QuadDisplayer.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin
{

	QuadDisplayer::QuadDisplayer(const QuadDisplayerBuilder *quadDisplayerBuilder) :
			isInitialized(false),
			numberOfQuad(quadDisplayerBuilder->getNumberOfQuad()),
			dimension(quadDisplayerBuilder->getDimension()),
			bufferUsage(quadDisplayerBuilder->getBufferUsage()),
			vertexDataType(quadDisplayerBuilder->getVertexDataType()),
			vertexCoord(quadDisplayerBuilder->getVertexCoord()),
			textureDataType(quadDisplayerBuilder->getTextureDataType()),
			textureCoord(quadDisplayerBuilder->getTextureCoord()),
			vertexArrayObject(0)
	{
		if(vertexDataType!=GL_FLOAT && vertexDataType!=GL_INT)
		{
			throw std::invalid_argument("Vertex data type not supported: " + vertexDataType);
		}else if(textureDataType!=GL_FLOAT && textureDataType!=GL_INT)
		{
			throw std::invalid_argument("Texture data type not supported: " + textureDataType);
		}
	}

	QuadDisplayer::~QuadDisplayer()
	{
		if(vertexArrayObject!=0)
		{
			glDeleteVertexArrays(1, &vertexArrayObject);
		}

		if(bufferIDs!=nullptr)
		{
			glDeleteBuffers(2, bufferIDs);
		}
	}

	void QuadDisplayer::initialize()
	{
		#ifdef _DEBUG
			if(isInitialized)
			{
				throw std::runtime_error("Quad displayer is already initialized.");
			}
		#endif

		glGenBuffers(2, bufferIDs);
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
		const unsigned int vertexSize = (vertexDataType==GL_FLOAT ? sizeof(float) : sizeof(int)) * 4*dimension*numberOfQuad;
		glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexCoord, bufferUsage);
		glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
		glVertexAttribPointer(SHADER_VERTEX_POSITION, dimension, vertexDataType, false, 0, 0);
		(vertexDataType==GL_FLOAT) ?
				(delete [] static_cast<float *>(vertexCoord)) :
				(delete [] static_cast<int *>(vertexCoord));
		vertexCoord = nullptr;

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
		const unsigned int textureSize = (textureDataType==GL_FLOAT ? sizeof(float) : sizeof(int)) * 4*dimension*numberOfQuad;
		glBufferData(GL_ARRAY_BUFFER, textureSize, textureCoord, bufferUsage);
		glEnableVertexAttribArray(SHADER_TEX_COORD);
		glVertexAttribPointer(SHADER_TEX_COORD, dimension, textureDataType, false, 0, 0);
		(textureDataType==GL_FLOAT) ?
						(delete [] static_cast<float *>(textureCoord)) :
						(delete [] static_cast<int *>(textureCoord));
		textureCoord = nullptr;

		isInitialized = true;
	}

	void QuadDisplayer::display() const
	{
		#ifdef _DEBUG
			if(!isInitialized)
			{
				throw std::runtime_error("Quad displayer must be initialized before display.");
			}
		#endif

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_QUADS, 0, 4*numberOfQuad);

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}

}
