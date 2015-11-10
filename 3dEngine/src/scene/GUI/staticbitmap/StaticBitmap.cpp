#include <GL/gl.h>

#include "scene/GUI/staticbitmap/StaticBitmap.h"
#include "resources/MediaManager.h"

namespace urchin
{
	
	StaticBitmap::StaticBitmap(int positionX, int positionY, const std::string &filename) :
		Widget(positionX, positionY, -1, -1)
	{
		//loads the texture
		tex = MediaManager::instance()->getMedia<Image>(filename);
		tex->toTexture(false, false);
		
		width = tex->getWidth();
		height = tex->getHeight();

		//visual
		glGenBuffers(2, bufferIDs);
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		const int vertexArray[] = {0, 0, width, 0, width, height, 0, height};
		const float stArray[] = {0.0, 0.0, tex->getMaxCoordS(), 0.0, tex->getMaxCoordS(), tex->getMaxCoordT(), 0.0, tex->getMaxCoordT()};

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
		glVertexAttribPointer(SHADER_VERTEX_POSITION, 2, GL_INT, false, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(stArray), stArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(SHADER_TEX_COORD);
		glVertexAttribPointer(SHADER_TEX_COORD, 2, GL_FLOAT, false, 0, 0);
	}

	StaticBitmap::~StaticBitmap()
	{
		glDeleteVertexArrays(1, &vertexArrayObject);
		glDeleteBuffers(2, bufferIDs);

		tex->release();
	}

	void StaticBitmap::display(int translateDistanceLoc, float invFrameRate)
	{
		glBindTexture(GL_TEXTURE_2D, tex->getTextureID());

		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_QUADS, 0, 4);

		//displays children
		Widget::display(translateDistanceLoc, invFrameRate);
	}

}
