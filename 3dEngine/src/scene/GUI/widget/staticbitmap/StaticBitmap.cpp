#include <GL/glew.h>

#include "scene/GUI/widget/staticbitmap/StaticBitmap.h"
#include "resources/MediaManager.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin
{
	
	StaticBitmap::StaticBitmap(Position position, Size size, const std::string &filename) :
		Widget(position, size),
		filename(filename),
		tex(nullptr)
	{
		createOrUpdateWidget();
	}

	StaticBitmap::~StaticBitmap()
	{
		tex->release();
	}

	void StaticBitmap::createOrUpdateWidget()
	{
		//loads the texture
		if(tex!=nullptr)
		{
			tex->release();
		}
		tex = MediaManager::instance()->getMedia<Image>(filename);
		tex->toTexture(false, false);

		//visual
		quadDisplayer = std::make_unique<QuadDisplayerBuilder>()
				->vertexData(GL_UNSIGNED_INT, new unsigned int[8]{0, 0, getWidth(), 0, getWidth(), getHeight(), 0, getHeight()})
				->textureData(GL_FLOAT, new float[8]{0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0})
				->build();
	}

	void StaticBitmap::display(int translateDistanceLoc, float invFrameRate)
	{
		glBindTexture(GL_TEXTURE_2D, tex->getTextureID());

		quadDisplayer->display();

		Widget::display(translateDistanceLoc, invFrameRate);
	}

}
