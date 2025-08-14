#include <utility>

#include "scene/ui/widget/staticbitmap/StaticBitmap.h"
#include "scene/ui/displayer/WidgetInstanceDisplayer.h"
#include "resources/ResourceRetriever.h"

namespace urchin {

    StaticBitmap::StaticBitmap(Position position, Size size, std::shared_ptr<Texture> texture) :
            Widget(std::move(position), size),
            texture(std::move(texture)) {

    }

    std::shared_ptr<StaticBitmap> StaticBitmap::create(Widget* parent, Position position, Size size, std::shared_ptr<Texture> texture) {
        return Widget::create<StaticBitmap>(new StaticBitmap(std::move(position), size, std::move(texture)), parent);
    }

    std::shared_ptr<StaticBitmap> StaticBitmap::create(Widget* parent, Position position, Size size, const std::string& filename) {
        std::shared_ptr<Texture> texture = buildTexture(filename);
        return Widget::create<StaticBitmap>(new StaticBitmap(std::move(position), size, std::move(texture)), parent);
    }

    std::shared_ptr<StaticBitmap> StaticBitmap::create(Widget* parent, Position position, WidthSize widthSize, const std::string& filename) {
        std::shared_ptr<Texture> texture = buildTexture(filename);
        float ratio = (float)texture->getHeight() / (float)texture->getWidth();
        Size size(widthSize.getWidth(), widthSize.getWidthType(), ratio, RATIO_TO_WIDTH);
        return Widget::create<StaticBitmap>(new StaticBitmap(std::move(position), size, std::move(texture)), parent);
    }

    std::shared_ptr<StaticBitmap> StaticBitmap::create(Widget* parent, Position position, HeightSize heightSize, const std::string& filename) {
        std::shared_ptr<Texture> texture = buildTexture(filename);
        float ratio = (float)texture->getWidth() / (float)texture->getHeight();
        Size size(ratio, RATIO_TO_HEIGHT, heightSize.getHeight(), heightSize.getHeightType());
        return Widget::create<StaticBitmap>(new StaticBitmap(std::move(position), size, std::move(texture)), parent);
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
        std::size_t implementationInstanceId = 0;
        HashUtil::combine(implementationInstanceId, texture->getId());
        return implementationInstanceId;
    }

    WidgetType StaticBitmap::getWidgetType() const {
        return WidgetType::STATIC_BITMAP;
    }

    void StaticBitmap::updateTexture(const std::string& filename) {
        texture = buildTexture(filename);
        changeTexture(texture);
    }

    const std::string& StaticBitmap::getTextureName() const {
        return texture->getName();
    }

}
