#include <utility>
#include <UrchinCommon.h>

#include <scene/ui/widget/button/Button.h>
#include <scene/ui/UISkinService.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    Button::Button(Position position, Size size, std::string skinName, std::string buttonText) :
            Widget(position, size),
            skinName(std::move(skinName)),
            text(nullptr),
            buttonText(std::move(buttonText)) {

    }

    std::shared_ptr<Button> Button::create(Widget* parent, Position position, Size size, std::string skinName, std::string buttonText) {
        return Widget::create<Button>(new Button(position, size, std::move(skinName), std::move(buttonText)), parent);
    }

    void Button::createOrUpdateWidget() {
        //detach children
        detachChild(text.get());

        //skin information
        auto buttonChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "button", UdaAttribute("skin", skinName));

        auto skinDefaultChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "default"), buttonChunk);
        texInfoDefault = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinDefaultChunk);
        currentTexture = texInfoDefault;

        auto skinFocusChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "focus"), buttonChunk);
        texInfoOnFocus = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinFocusChunk);

        auto skinClickChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "click"), buttonChunk);
        texInfoOnClick = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinClickChunk);

        if (!buttonText.empty()) {
            auto textSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textSkin", UdaAttribute(), buttonChunk);
            text = Text::create(this, Position(0.0f, 0.0f, LengthType::PIXEL), textSkinChunk->getStringValue(), i18n(buttonText));
        }

        //visual
        setupRenderer(baseRendererBuilder("button", ShapeType::TRIANGLE, false)
                ->addUniformTextureReader(TextureReader::build(currentTexture, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy()))) //binding 3
                ->build());
    }

    WidgetType Button::getWidgetType() const {
        return WidgetType::BUTTON;
    }

    void Button::refreshTexture() {
        auto oldTexture = currentTexture;
        if (getWidgetState() == FOCUS) {
            currentTexture = texInfoOnFocus;
        } else if (getWidgetState() == CLICKING) {
            currentTexture = texInfoOnClick;
        } else {
            currentTexture = texInfoDefault;
        }

        if (currentTexture != oldTexture) {
            getRenderer()->updateUniformTextureReader(0, TextureReader::build(currentTexture, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy())));
        }
    }

    bool Button::onKeyPressEvent(InputDeviceKey) {
        refreshTexture();
        return true;
    }

    bool Button::onKeyReleaseEvent(InputDeviceKey) {
        refreshTexture();
        return true;
    }

    bool Button::onMouseMoveEvent(int, int) {
        refreshTexture();
        return true;
    }

    void Button::prepareWidgetRendering(float, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        if (text) {
            //update the text position because the text size is updated when the UI language is changed
            text->updatePosition(Position((getWidth() - text->getWidth()) / 2.0f, (getHeight() - text->getHeight()) / 2.0f, LengthType::PIXEL));
        }

        updateProperties(getRenderer(), projectionViewMatrix, Vector2<float>(getGlobalPositionX(), getGlobalPositionY()));
        getRenderer()->enableRenderer(renderingOrder);
    }

}
