#include <GL/glew.h>

#include <utility>

#include "scene/GUI/widget/staticbitmap/StaticBitmap.h"
#include "resources/MediaManager.h"
#include "graphic/displayer/generic/GenericDisplayerBuilder.h"

namespace urchin {

    StaticBitmap::StaticBitmap(Position position, Size size, std::string filename) :
        Widget(position, size),
        filename(std::move(filename)),
        tex(nullptr) {
        StaticBitmap::createOrUpdateWidget();
    }

    StaticBitmap::~StaticBitmap() {
        tex->release();
    }

    void StaticBitmap::createOrUpdateWidget() {
        //loads the texture
        if (tex) {
            tex->release();
        }
        tex = MediaManager::instance()->getMedia<Image>(filename);
        tex->toTexture(false, false, false);

        //visual
        bitmapDisplayer = std::make_unique<GenericDisplayerBuilder>()
                ->vertexData(GL_UNSIGNED_INT, new unsigned int[8]{0, 0, getWidth(), 0, getWidth(), getHeight(), 0, getHeight()}, true)
                ->textureData(GL_FLOAT, new float[8]{0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0}, true)
                ->addTextureId(tex->getTextureID())
                ->build();
    }

    void StaticBitmap::display(int translateDistanceLoc, float dt) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        bitmapDisplayer->display();

        glDisable(GL_BLEND);

        Widget::display(translateDistanceLoc, dt);
    }

}
