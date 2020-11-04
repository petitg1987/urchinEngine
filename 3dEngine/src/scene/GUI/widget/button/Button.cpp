#include <utility>
#include "UrchinCommon.h"

#include "scene/GUI/widget/button/Button.h"
#include "scene/GUI/GUISkinService.h"
#include "graphic/displayer/generic/GenericDisplayerBuilder.h"

namespace urchin {

    Button::Button(Position position, Size size, std::string nameSkin, std::string buttonText)
        : Widget(position, size),
          nameSkin(std::move(nameSkin)),
          text(nullptr),
          buttonText(std::move(buttonText)) {
        Button::createOrUpdateWidget();
    }

    void Button::createOrUpdateWidget() {
        //skin information
        std::shared_ptr<XmlChunk> buttonChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "button", XmlAttribute("nameSkin", nameSkin));

        std::shared_ptr<XmlChunk> skinDefaultChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "default"), buttonChunk);
        texInfoDefault = GUISkinService::instance()->createTexWidget(getWidth(), getHeight(), skinDefaultChunk);

        std::shared_ptr<XmlChunk> skinFocusChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "focus"), buttonChunk);
        texInfoOnFocus = GUISkinService::instance()->createTexWidget(getWidth(), getHeight(), skinFocusChunk);

        std::shared_ptr<XmlChunk> skinClickChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "click"), buttonChunk);
        texInfoOnClick = GUISkinService::instance()->createTexWidget(getWidth(), getHeight(), skinClickChunk);

        if (!buttonText.empty()) {
            std::shared_ptr<XmlChunk> textFontChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "textFont", XmlAttribute(), buttonChunk);
            removeChild(text);
            text = new Text(Position(0, 0, Position::PIXEL), textFontChunk->getStringValue());
            text->setText(buttonText);
            text->setPosition(Position((float)(getWidth() - text->getWidth()) / 2.0f, (float)(getHeight() - text->getHeight()) / 2.0f, Position::PIXEL));
            addChild(text);
        }

        //visual
        buttonDisplayer = std::make_unique<GenericDisplayerBuilder>(ShapeType::RECTANGLE)
                ->vertexData(CoordDataType::UNSIGNED_INT, new unsigned int[8]{0, 0, getWidth(), 0, getWidth(), getHeight(), 0, getHeight()}, true)
                ->textureData(CoordDataType::FLOAT, new float[8]{0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0}, true)
                ->addTexture(Texture::build(texInfoDefault->getTextureID()))
                ->build();
    }

    unsigned int Button::getTextureId() {
        if (getWidgetState()==FOCUS) {
            return texInfoOnFocus->getTextureID();
        } else if (getWidgetState()==CLICKING) {
            return texInfoOnClick->getTextureID();
        }

        return texInfoDefault->getTextureID();
    }

    bool Button::onKeyPressEvent(unsigned int) {
        buttonDisplayer->updateTexture(0, Texture::build(getTextureId()));
        return true;
    }

    bool Button::onKeyReleaseEvent(unsigned int) {
        buttonDisplayer->updateTexture(0, Texture::build(getTextureId()));
        return true;
    }

    bool Button::onMouseMoveEvent(int, int) {
        buttonDisplayer->updateTexture(0, Texture::build(getTextureId()));
        return true;
    }

    void Button::display(int translateDistanceLoc, float dt) {
        buttonDisplayer->display();

        Widget::display(translateDistanceLoc, dt);
    }

}
