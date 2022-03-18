#include <utility>

#include <scene/ui/widget/staticbitmap/StaticBitmap.h>
#include <resources/ResourceRetriever.h>
#include <api/render/GenericRendererBuilder.h>

namespace urchin {

    StaticBitmap::StaticBitmap(Position position, Size size, std::shared_ptr<Image> image) :
            Widget(position, size),
            image(std::move(image)),
            tex(nullptr) {

    }

    std::shared_ptr<StaticBitmap> StaticBitmap::create(Widget* parent, Position position, Size size, const std::string& filename) {
        std::shared_ptr<Image> image = ResourceRetriever::instance().getResource<Image>(filename);
        return Widget::create<StaticBitmap>(new StaticBitmap(position, size, std::move(image)), parent);
    }

    std::shared_ptr<StaticBitmap> StaticBitmap::create(Widget* parent, Position position, Size size, std::shared_ptr<Image> image) {
        return Widget::create<StaticBitmap>(new StaticBitmap(position, size, std::move(image)), parent);
    }

    void StaticBitmap::createOrUpdateWidget() {
        //create the texture
        tex = image->createTexture(true);

        //visual
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>((float)getWidth(), 0.0f), Point2<float>((float)getWidth(), (float)getHeight()),
                Point2<float>(0.0f, 0.0f), Point2<float>((float)getWidth(), (float)getHeight()), Point2<float>(0.0f, (float)getHeight())
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        bitmapRenderer = setupUiRenderer("static bitmap", ShapeType::TRIANGLE, true)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(tex, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy()))) //binding 2
                ->build();
    }

    WidgetType StaticBitmap::getWidgetType() const {
        return WidgetType::STATIC_BITMAP;
    }

    const std::string& StaticBitmap::getImageName() const {
        return image->getName();
    }

    void StaticBitmap::prepareWidgetRendering(float, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        updatePositioning(bitmapRenderer.get(), projectionViewMatrix, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        bitmapRenderer->enableRenderer(renderingOrder);
    }

}
