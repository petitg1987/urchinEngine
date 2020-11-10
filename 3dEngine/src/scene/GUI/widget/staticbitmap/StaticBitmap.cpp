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
        std::vector<unsigned int> vertexCoord = {0, 0, getWidth(), 0, getWidth(), getHeight(), 0, getHeight()};
        std::vector<float> textureCoord = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0};
        bitmapRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::RECTANGLE)
                ->vertexData(CoordType::UNSIGNED_INT, CoordDimension::_2D, &vertexCoord[0])
                ->textureData(CoordType::FLOAT, CoordDimension::_2D, &textureCoord[0])
                ->addTexture(Texture::build(tex->getTextureID()))
                ->enableTransparency()
                ->build();
    }

    void StaticBitmap::display(const std::unique_ptr<Shader> &guiShader, const ShaderVar &translateDistanceShaderVar, float dt) {
        bitmapRenderer->draw();

        Widget::display(guiShader, translateDistanceShaderVar, dt);
    }

}
