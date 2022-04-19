#include <scene/ui/widget/checkbox/CheckBox.h>
#include <resources/ResourceRetriever.h>

namespace urchin {

    CheckBox::CheckBox(Position position, Size size, std::string skinName) :
            Widget(position, size),
            skinName(std::move(skinName)),
            bIsChecked(false) {

    }

    std::shared_ptr<CheckBox> CheckBox::create(Widget* parent, Position position, Size size, std::string skinName) {
        return Widget::create<CheckBox>(new CheckBox(position, size, std::move(skinName)), parent);
    }

    void CheckBox::createOrUpdateWidget() {
        //skin information
        auto checkBoxChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "checkBox", UdaAttribute("skin", skinName));
        texChecked = loadTexture(checkBoxChunk, "imageChecked");
        texUnchecked = loadTexture(checkBoxChunk, "imageUnchecked");

        //visual
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(getWidth(), 0.0f), Point2<float>(getWidth(), getHeight()),
                Point2<float>(0.0f, 0.0f), Point2<float>(getWidth(), getHeight()), Point2<float>(0.0f, getHeight())
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        checkBoxRenderer = setupUiRenderer("check box", ShapeType::TRIANGLE, true)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(isChecked() ? texChecked : texUnchecked, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy()))) //binding 3
                ->build();
    }

    WidgetType CheckBox::getWidgetType() const {
        return WidgetType::CHECKBOX;
    }

    std::shared_ptr<Texture> CheckBox::loadTexture(const UdaChunk* checkBoxChunk, std::string_view chunkName) const {
        auto imageElem = UISkinService::instance().getSkinReader().getFirstChunk(true, chunkName, UdaAttribute(), checkBoxChunk);
        return ResourceRetriever::instance().getResource<Texture>(imageElem->getStringValue(), {{"mipMap", "0"}});
    }

    void CheckBox::setChecked(bool isChecked) {
        this->bIsChecked = isChecked;
        refreshTexture();
    }

    bool CheckBox::isChecked() const {
        return bIsChecked;
    }

    void CheckBox::refreshTexture() const {
        if (checkBoxRenderer) {
            if (bIsChecked) {
                checkBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texChecked, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy())));
            } else {
                checkBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texUnchecked, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy())));
            }
        }
    }

    bool CheckBox::onKeyReleaseEvent(unsigned int) {
        if (getWidgetState() == FOCUS) {
            switchValue();
            return false;
        }
        return true;
    }

    void CheckBox::switchValue() {
        setChecked(!bIsChecked);

        for (auto& eventListener : getEventListeners()) {
            eventListener->onValueChange(this);
        }
    }

    void CheckBox::prepareWidgetRendering(float, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        updatePositioning(checkBoxRenderer.get(), projectionViewMatrix, Vector2<float>(getGlobalPositionX(), getGlobalPositionY()));
        checkBoxRenderer->enableRenderer(renderingOrder);
    }
}
