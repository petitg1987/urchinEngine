#include <GL/gl.h>

#include "scene/GUI/staticbitmap/StaticBitmap.h"
#include "resources/MediaManager.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin
{
	
	StaticBitmap::StaticBitmap(int positionX, int positionY, const std::string &filename) :
		Widget(positionX, positionY, -1, -1)
	{
		//loads the texture
		tex = MediaManager::instance()->getMedia<Image>(filename);
		tex->toTexture(false, false);
		
		width = 1900; //tex->getWidth();
		height = 1080; //tex->getHeight();

		//visual
		quadDisplayer = std::make_unique<QuadDisplayerBuilder>()
				->vertexData(GL_INT, new int[8]{0, 0, width, 0, width, height, 0, height})
				->textureData(GL_FLOAT, new float[8]{0.0, 0.0, tex->getMaxCoordS(), 0.0, tex->getMaxCoordS(), tex->getMaxCoordT(), 0.0, tex->getMaxCoordT()})
				->build();
	}

	StaticBitmap::~StaticBitmap()
	{
		tex->release();
	}

	void StaticBitmap::display(int translateDistanceLoc, float invFrameRate)
	{
		glBindTexture(GL_TEXTURE_2D, tex->getTextureID());

		quadDisplayer->display();

		//displays children
		Widget::display(translateDistanceLoc, invFrameRate);
	}

}
