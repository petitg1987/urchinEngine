#include <GL/gl.h>

#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin
{

	QuadDisplayerBuilder::QuadDisplayerBuilder() :
		pBufferUsage(GL_STATIC_DRAW),
		vertexDataType(GL_INT),
		vertexCoord(nullptr),
		textureDataType(GL_INT),
		textureCoord(nullptr)
	{

	}

	QuadDisplayerBuilder::~QuadDisplayerBuilder()
	{

	}

	QuadDisplayerBuilder *QuadDisplayerBuilder::bufferUsage(unsigned int bufferUsage)
	{
		this->pBufferUsage = bufferUsage;
		return this;
	}

	unsigned int QuadDisplayerBuilder::getBufferUsage() const
	{
		return pBufferUsage;
	}

	QuadDisplayerBuilder *QuadDisplayerBuilder::vertexData(unsigned int vertexDataType, void *vertexCoord)
	{
		this->vertexDataType = vertexDataType;
		this->vertexCoord = vertexCoord;
		return this;
	}

	unsigned int QuadDisplayerBuilder::getVertexDataType() const
	{
		return vertexDataType;
	}

	void *QuadDisplayerBuilder::getVertexCoord() const
	{
		return vertexCoord;
	}

	QuadDisplayerBuilder *QuadDisplayerBuilder::textureData(unsigned int textureDataType, void *textureCoord)
	{
		this->textureDataType = textureDataType;
		this->textureCoord = textureCoord;
		return this;
	}

	unsigned int QuadDisplayerBuilder::getTextureDataType() const
	{
		return textureDataType;
	}

	void *QuadDisplayerBuilder::getTextureCoord() const
	{
		return textureCoord;
	}

	std::shared_ptr<QuadDisplayer> QuadDisplayerBuilder::build()
	{
		if(vertexCoord==nullptr)
		{
			vertexCoord = new int[8]{
					-1, 1,
					1, 1,
					1, -1,
					-1, -1};
		}

		if(textureCoord==nullptr)
		{
			textureCoord = new int[8]{
					0, 1,
					1, 1,
					1, 0,
					0, 0};
		}

		std::shared_ptr<QuadDisplayer> quadDisplayer = std::make_shared<QuadDisplayer>(this);
		quadDisplayer->initialize();

		return quadDisplayer;
	}

}
