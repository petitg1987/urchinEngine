#include <utility>
#include <UrchinCommon.h>

#include <scene/ui/widget/button/Button.h>
#include <scene/ui/UISkinService.h>
#include <scene/ui/displayer/WidgetInstanceDisplayer.h>

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
        updateTexture(texInfoDefault);

        auto skinFocusChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "focus"), buttonChunk);
        texInfoOnFocus = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinFocusChunk);

        auto skinClickChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "click"), buttonChunk);
        texInfoOnClick = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinClickChunk);

        if (!buttonText.empty()) {
            auto textSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textSkin", UdaAttribute(), buttonChunk);
            text = Text::create(this, Position(0.0f, 0.0f, LengthType::PIXEL), textSkinChunk->getStringValue(), i18n(buttonText));
        }
    }

    bool Button::requireRenderer() const {
        return true;
    }

    WidgetType Button::getWidgetType() const {
        return WidgetType::BUTTON;
    }

    void Button::refreshTexture() {
        if (getWidgetState() == FOCUS) {
            if (getTexture().get() != texInfoOnFocus.get()) {
                updateTexture(texInfoOnFocus);
            }
        } else if (getWidgetState() == CLICKING) {
            if (getTexture().get() != texInfoOnClick.get()) {
                updateTexture(texInfoOnClick);
            }
        } else {
            if (getTexture().get() != texInfoDefault.get()) {
                updateTexture(texInfoDefault);
            }
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

    void Button::prepareWidgetRendering(float) {
        if (text) {
            //update the text position because the text size is updated when the UI language is changed
            text->updatePosition(Position((getWidth() - text->getWidth()) / 2.0f, (getHeight() - text->getHeight()) / 2.0f, LengthType::PIXEL));
        }
    }

}
