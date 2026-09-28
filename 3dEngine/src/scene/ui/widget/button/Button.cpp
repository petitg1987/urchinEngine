#include <utility>
#include <UrchinCommon.h>

#include "scene/ui/widget/button/Button.h"
#include "scene/ui/UISkinService.h"

namespace urchin {

    Button::Button(Position position, Size size, std::string skinName, std::string buttonText) :
            Widget(std::move(position), size),
            skinName(std::move(skinName)),
            text(nullptr),
            buttonText(std::move(buttonText)) {

    }

    std::shared_ptr<Button> Button::create(Widget* parent, Position position, Size size, std::string skinName, std::string buttonText) {
        return Widget::create<Button>(new Button(std::move(position), size, std::move(skinName), std::move(buttonText)), parent);
    }

    void Button::createOrUpdateWidget() {
        //detach children
        detachChild(text.get());

        //skin information
        auto buttonChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "button", UdaAttribute("skin", skinName));

        auto skinDefaultChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "default"), buttonChunk);
        texDefault = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinDefaultChunk);
        changeTexture(texDefault);

        auto skinFocusChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "focus"), buttonChunk);
        texOnFocus = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinFocusChunk);

        auto skinClickChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "click"), buttonChunk);
        texOnClick = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinClickChunk);

        if (!buttonText.empty()) {
            auto textSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textSkin", UdaAttribute(), buttonChunk);
            text = Text::create(this, Position(0.0f, 0.0f, PIXEL), textSkinChunk->getStringValue(), buttonText);
        }
    }

    bool Button::requireRenderer() const {
        return true;
    }

    WidgetType Button::getWidgetType() const {
        return WidgetType::BUTTON;
    }

    bool Button::refreshTexture() {
        bool textureRefreshed = false;
        if (getWidgetState() == FOCUS) {
            if (getTexture().get() != texOnFocus.get()) {
                changeTexture(texOnFocus);
                textureRefreshed = true;
            }
        } else if (getWidgetState() == CLICKING) {
            if (getTexture().get() != texOnClick.get()) {
                changeTexture(texOnClick);
                textureRefreshed = true;
            }
        } else {
            if (getTexture().get() != texDefault.get()) {
                changeTexture(texDefault);
                textureRefreshed = true;
            }
        }
        return textureRefreshed;
    }

    bool Button::onKeyPressEvent(InputKey) {
        return !refreshTexture();
    }

    bool Button::onKeyReleaseEvent(InputKey) {
        return !refreshTexture();
    }

    bool Button::onMouseMoveEvent(int, int) {
        return !refreshTexture();
    }

    void Button::onGameControllerFocusUpdate() {
        refreshTexture();
    }

    void Button::prepareWidgetRendering(float) {
        if (text) {
            //update the text position because the text size is updated when the UI language is changed
            text->updatePosition(Position((getWidth() - text->getWidth()) / 2.0f, (getHeight() - text->getHeight()) / 2.0f, PIXEL));
        }
    }

}
