#include <utility>

#include <scene/ui/widget/staticbitmap/StaticBitmap.h>
#include <resources/MediaManager.h>
#include <graphic/render/GenericRendererBuilder.h>

namespace urchin {

    StaticBitmap::StaticBitmap(Position position, Size size, std::string filename) :
            Widget(position, size),
            filename(std::move(filename)),
            tex(nullptr) {

    }

    std::shared_ptr<StaticBitmap> StaticBitmap::newStaticBitmap(Widget* parent, Position position, Size size, std::string filename) {
        return create<StaticBitmap>(new StaticBitmap(position, size, std::move(filename)), parent);
    }

    void StaticBitmap::createOrUpdateWidget() {
        //loads the texture
        auto img = MediaManager::instance()->getMedia<Image>(filename);
        tex = img->createTexture(false);

        //visual
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>((float)getWidth(), 0.0f), Point2<float>((float)getWidth(), (float)getHeight()),
                Point2<float>(0.0f, 0.0f), Point2<float>((float)getWidth(), (float)getHeight()), Point2<float>(0.0f, (float)getHeight())
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        bitmapRenderer = setupUiRenderer("static bitmap", ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(tex, TextureParam::buildLinear())) //binding 2
                ->enableTransparency()
                ->build();
    }

    void StaticBitmap::prepareWidgetRendering(float) {
        updateTranslateVector(bitmapRenderer.get(), Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        bitmapRenderer->enableRenderer();
    }

}
