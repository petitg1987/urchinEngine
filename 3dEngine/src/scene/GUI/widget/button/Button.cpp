#include <GL/glew.h>

#include <utility>
#include "UrchinCommon.h"

#include "scene/GUI/widget/button/Button.h"
#include "scene/GUI/GUISkinService.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin {

    Button::Button(Position position, Size size, std::string nameSkin, std::string buttonText)
        : Widget(position, size),
          nameSkin(std::move(nameSkin)),
          text(nullptr),
          buttonText(std::move(buttonText)),
          textureID(0) {
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
        quadDisplayer = std::make_unique<QuadDisplayerBuilder>()
                ->vertexData(GL_UNSIGNED_INT, new unsigned int[8]{0, 0, getWidth(), 0, getWidth(), getHeight(), 0, getHeight()}, true)
                ->textureData(GL_FLOAT, new float[8]{0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0}, true)
                ->build();

        textureID = texInfoDefault->getTextureID();
    }

    unsigned int Button::getTextureId() {
        if (getWidgetState()==FOCUS) {
            return texInfoOnFocus->getTextureID();
        } else if (getWidgetState()==CLICKING) {
            textureID = texInfoOnClick->getTextureID();
        }

        return texInfoDefault->getTextureID();
    }

    bool Button::onKeyPressEvent(unsigned int) {
        textureID = getTextureId();
        return true;
    }

    bool Button::onKeyReleaseEvent(unsigned int) {
        textureID = getTextureId();
        return true;
    }

    bool Button::onMouseMoveEvent(int, int) {
        textureID = getTextureId();
        return true;
    }

    void Button::display(int translateDistanceLoc, float dt) {
        glBindTexture(GL_TEXTURE_2D, textureID);

        quadDisplayer->display();

        Widget::display(translateDistanceLoc, dt);
    }

}
