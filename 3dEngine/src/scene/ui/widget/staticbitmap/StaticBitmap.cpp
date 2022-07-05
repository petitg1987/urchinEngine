#include <utility>

#include <scene/ui/widget/staticbitmap/StaticBitmap.h>
#include <resources/ResourceRetriever.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    StaticBitmap::StaticBitmap(Position position, Size size, std::shared_ptr<Texture> texture) :
            Widget(position, size),
            texture(std::move(texture)) {

    }

    std::shared_ptr<StaticBitmap> StaticBitmap::create(Widget* parent, Position position, Size size, std::shared_ptr<Texture> texture) {
        return Widget::create<StaticBitmap>(new StaticBitmap(position, size, std::move(texture)), parent);
    }

    std::shared_ptr<StaticBitmap> StaticBitmap::create(Widget* parent, Position position, Size size, const std::string& filename) {
        std::shared_ptr<Texture> texture = buildTexture(filename);
        return Widget::create<StaticBitmap>(new StaticBitmap(position, size, std::move(texture)), parent);
    }

    std::shared_ptr<StaticBitmap> StaticBitmap::create(Widget* parent, Position position, WidthSize widthSize, const std::string& filename) {
        std::shared_ptr<Texture> texture = buildTexture(filename);
        float ratio = (float)texture->getHeight() / (float)texture->getWidth();
        Size size(widthSize.getWidth(), widthSize.getWidthType(), ratio, RATIO_TO_WIDTH);
        return Widget::create<StaticBitmap>(new StaticBitmap(position, size, std::move(texture)), parent);
    }

    std::shared_ptr<StaticBitmap> StaticBitmap::create(Widget* parent, Position position, HeightSize heightSize, const std::string& filename) {
        std::shared_ptr<Texture> texture = buildTexture(filename);
        float ratio = (float)texture->getWidth() / (float)texture->getHeight();
        Size size(ratio, RATIO_TO_HEIGHT, heightSize.getHeight(), heightSize.getHeightType());
        return Widget::create<StaticBitmap>(new StaticBitmap(position, size, std::move(texture)), parent);
    }

    std::shared_ptr<Texture> StaticBitmap::buildTexture(const std::string& filename) {
        if (filename.empty()) {
            return Texture::buildEmptyRgba("empty texture");
        }
        return ResourceRetriever::instance().getResource<Texture>(filename, {{"mipMap", "1"}});
    }

    void StaticBitmap::createOrUpdateWidget() {
        //visual
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(getWidth(), 0.0f), Point2<float>(getWidth(), getHeight()),
                Point2<float>(0.0f, 0.0f), Point2<float>(getWidth(), getHeight()), Point2<float>(0.0f, getHeight())
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        bitmapRenderer = setupUiRenderer("static bitmap", ShapeType::TRIANGLE, true)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(texture, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy()))) //binding 3
                ->build();
    }

    WidgetType StaticBitmap::getWidgetType() const {
        return WidgetType::STATIC_BITMAP;
    }

    void StaticBitmap::updateTexture(const std::string& filename) {
        texture = ResourceRetriever::instance().getResource<Texture>(filename, {{"mipMap", "1"}});
        bitmapRenderer->updateUniformTextureReader(0, TextureReader::build(texture, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy())));
    }

    const std::string& StaticBitmap::getTextureName() const {
        return texture->getName();
    }

    void StaticBitmap::prepareWidgetRendering(float, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        updateProperties(bitmapRenderer.get(), projectionViewMatrix, Vector2<float>(getGlobalPositionX(), getGlobalPositionY()));
        bitmapRenderer->enableRenderer(renderingOrder);
    }

}
