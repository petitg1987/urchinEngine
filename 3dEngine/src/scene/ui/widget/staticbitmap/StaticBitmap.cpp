#include <utility>

#include "scene/ui/widget/staticbitmap/StaticBitmap.h"
#include "scene/ui/displayer/WidgetInstanceDisplayer.h"
#include "resources/ResourceRetriever.h"

namespace urchin {

    StaticBitmap::StaticBitmap(Position position, Size size, std::shared_ptr<Texture> texture, std::optional<Size> repeatTileSize) :
            Widget(std::move(position), size),
            texture(std::move(texture)),
            repeatTileSize(repeatTileSize) {

    }

    std::shared_ptr<StaticBitmap> StaticBitmap::create(Widget* parent, Position position, Size size, std::shared_ptr<Texture> texture) {
        return Widget::create<StaticBitmap>(new StaticBitmap(std::move(position), size, std::move(texture), std::nullopt), parent);
    }

    std::shared_ptr<StaticBitmap> StaticBitmap::create(Widget* parent, Position position, Size size, const std::string& filename) {
        std::shared_ptr<Texture> texture = buildTexture(filename);
        return Widget::create<StaticBitmap>(new StaticBitmap(std::move(position), size, std::move(texture), std::nullopt), parent);
    }

    std::shared_ptr<StaticBitmap> StaticBitmap::create(Widget* parent, Position position, WidthSize widthSize, const std::string& filename) {
        std::shared_ptr<Texture> texture = buildTexture(filename);
        float ratio = (float)texture->getHeight() / (float)texture->getWidth();
        Size size(widthSize.getWidth(), widthSize.getWidthType(), ratio, RATIO_TO_WIDTH);
        return Widget::create<StaticBitmap>(new StaticBitmap(std::move(position), size, std::move(texture), std::nullopt), parent);
    }

    std::shared_ptr<StaticBitmap> StaticBitmap::create(Widget* parent, Position position, HeightSize heightSize, const std::string& filename) {
        std::shared_ptr<Texture> texture = buildTexture(filename);
        float ratio = (float)texture->getWidth() / (float)texture->getHeight();
        Size size(ratio, RATIO_TO_HEIGHT, heightSize.getHeight(), heightSize.getHeightType());
        return Widget::create<StaticBitmap>(new StaticBitmap(std::move(position), size, std::move(texture), std::nullopt), parent);
    }

    std::shared_ptr<StaticBitmap> StaticBitmap::createRepeated(Widget* parent, Position position, Size size, Size repeatTileSize, const std::string& filename) {
        std::shared_ptr<Texture> texture = buildTexture(filename);
        return Widget::create<StaticBitmap>(new StaticBitmap(std::move(position), size, std::move(texture), repeatTileSize), parent);
    }

    std::shared_ptr<StaticBitmap> StaticBitmap::createRepeated(Widget* parent, Position position, Size size, HeightSize repeatTileHeight, const std::string& filename) {
        std::shared_ptr<Texture> texture = buildTexture(filename);
        float ratio = (float)texture->getWidth() / (float)texture->getHeight();
        Size repeatTileSize(ratio, RATIO_TO_HEIGHT, repeatTileHeight.getHeight(), repeatTileHeight.getHeightType());
        return Widget::create<StaticBitmap>(new StaticBitmap(std::move(position), size, std::move(texture), repeatTileSize), parent);
    }

    std::shared_ptr<Texture> StaticBitmap::buildTexture(const std::string& filename) {
        if (filename.empty()) {
            return Texture::buildEmptyRgba8Bits("empty texture");
        }
        return ResourceRetriever::instance().getResource<Texture>(filename, {{"mipMap", "1"}});
    }

    void StaticBitmap::createOrUpdateWidget() {
        changeTexture(texture);
    }

    bool StaticBitmap::requireRenderer() const {
        return true;
    }

    std::size_t StaticBitmap::getImplementationInstanceId() const {
        if (repeatTileSize.has_value()) {
            return INSTANCING_DENY_ID;
        }
        std::size_t implementationInstanceId = 0;
        HashUtil::hashCombine(implementationInstanceId, texture->getId());
        return implementationInstanceId;
    }

    Point2<float> StaticBitmap::computeRepeatTileSizeInPixel() const {
        assert(repeatTileSize.has_value());
        if (repeatTileSize->getWidthType() == RATIO_TO_HEIGHT) {
            float tileHeight = heightLengthToPixel(repeatTileSize->getHeight(), repeatTileSize->getHeightType(), [] { return 0.0f; /* unreachable: Size cannot have both lengths relative to each other */ });
            return Point2(widthLengthToPixel(repeatTileSize->getWidth(), RATIO_TO_HEIGHT, [&] { return tileHeight; }), tileHeight);
        } else if (repeatTileSize->getHeightType() == RATIO_TO_WIDTH) {
            float tileWidth = widthLengthToPixel(repeatTileSize->getWidth(), repeatTileSize->getWidthType(), [] { return 0.0f; /* unreachable */ });
            return Point2(tileWidth, heightLengthToPixel(repeatTileSize->getHeight(), RATIO_TO_WIDTH, [&] { return tileWidth; }));
        }
        float tileWidth = widthLengthToPixel(repeatTileSize->getWidth(), repeatTileSize->getWidthType(), [] { return 0.0f; /* unreachable */ });
        float tileHeight = heightLengthToPixel(repeatTileSize->getHeight(), repeatTileSize->getHeightType(), [] { return 0.0f; /* unreachable */ });
        return Point2(tileWidth, tileHeight);
    }

    void StaticBitmap::retrieveTextureCoordinates(std::vector<Point2<float>>& textureCoord) {
        if (!repeatTileSize.has_value()) {
            Widget::retrieveTextureCoordinates(textureCoord);
            return;
        }

        assert(textureCoord.empty());
        Point2<float> repeatTileSizeInPixel = computeRepeatTileSizeInPixel();
        float maxU = getWidth() / repeatTileSizeInPixel.X;
        float maxV = getHeight() / repeatTileSizeInPixel.Y;

        textureCoord.reserve(6);

        textureCoord.emplace_back(0.0f, 0.0f);
        textureCoord.emplace_back(maxU, 0.0f);
        textureCoord.emplace_back(maxU, maxV);

        textureCoord.emplace_back(0.0f, 0.0f);
        textureCoord.emplace_back(maxU, maxV);
        textureCoord.emplace_back(0.0f, maxV);
    }

    TextureParam::ReadMode StaticBitmap::getTextureReadMode() const {
        return repeatTileSize.has_value() ? TextureParam::REPEAT : TextureParam::EDGE_CLAMP;
    }

    WidgetType StaticBitmap::getWidgetType() const {
        return WidgetType::STATIC_BITMAP;
    }

    void StaticBitmap::updateSize(Size size) {
        Widget::updateSize(size);
    }

    void StaticBitmap::updateSize(WidthSize widthSize) {
        float ratio = (float)texture->getHeight() / (float)texture->getWidth();
        Widget::updateSize(Size(widthSize.getWidth(), widthSize.getWidthType(), ratio, RATIO_TO_WIDTH));
    }

    void StaticBitmap::updateSize(HeightSize heightSize) {
        float ratio = (float)texture->getWidth() / (float)texture->getHeight();
        Widget::updateSize(Size(ratio, RATIO_TO_HEIGHT, heightSize.getHeight(), heightSize.getHeightType()));
    }

    void StaticBitmap::updateTexture(const std::string& filename) {
        updateTexture(buildTexture(filename));
    }

    void StaticBitmap::updateTexture(std::shared_ptr<Texture> texture) {
        this->texture = std::move(texture);
        changeTexture(this->texture);
        if (repeatTileSize.has_value()) {
            notifyObservers(this, SIZE_UPDATED); //texture coordinates depend on the texture proportion
        }
    }

    const std::string& StaticBitmap::getTextureName() const {
        return texture->getName();
    }

}
