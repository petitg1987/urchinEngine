#include <utility>

#include "scene/GUI/widget/staticbitmap/StaticBitmap.h"
#include "resources/MediaManager.h"
#include "graphic/render/GenericRendererBuilder.h"

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
        bitmapRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::RECTANGLE)
                ->vertexData(CoordDataType::UNSIGNED_INT, new unsigned int[8]{0, 0, getWidth(), 0, getWidth(), getHeight(), 0, getHeight()}, true)
                ->textureData(CoordDataType::FLOAT, new float[8]{0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0}, true)
                ->addTexture(Texture::build(tex->getTextureID()))
                ->enableTransparency()
                ->build();
    }

    void StaticBitmap::display(int translateDistanceLoc, float dt) {
        bitmapRenderer->draw();

        Widget::display(translateDistanceLoc, dt);
    }

}
