#include <utility>
#include "UrchinCommon.h"

#include "scene/UI/widget/button/Button.h"
#include "scene/UI/UISkinService.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    Button::Button(Widget* parent, Position position, Size size, std::string nameSkin, std::string buttonText) :
            Widget(parent, position, size),
            nameSkin(std::move(nameSkin)),
            text(nullptr),
            buttonText(std::move(buttonText)) {

    }

    Button::Button(Position position, Size size, std::string nameSkin, std::string buttonText) :
            Button(nullptr, position, size, std::move(nameSkin), std::move(buttonText)) {

    }

    void Button::createOrUpdateWidget() {
        //skin information
        std::shared_ptr<XmlChunk> buttonChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "button", XmlAttribute("nameSkin", nameSkin));

        std::shared_ptr<XmlChunk> skinDefaultChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "default"), buttonChunk);
        texInfoDefault = UISkinService::instance()->createWidgetTexture(getWidth(), getHeight(), skinDefaultChunk);

        std::shared_ptr<XmlChunk> skinFocusChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "focus"), buttonChunk);
        texInfoOnFocus = UISkinService::instance()->createWidgetTexture(getWidth(), getHeight(), skinFocusChunk);

        std::shared_ptr<XmlChunk> skinClickChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "click"), buttonChunk);
        texInfoOnClick = UISkinService::instance()->createWidgetTexture(getWidth(), getHeight(), skinClickChunk);

        if (!buttonText.empty()) {
            std::shared_ptr<XmlChunk> textSkinChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "textSkin", XmlAttribute(), buttonChunk);
            delete text;
            text = new Text(this, Position(0, 0, LengthType::PIXEL), textSkinChunk->getStringValue(), buttonText);
            text->setPosition(Position(((float)getWidth() - (float)text->getWidth()) / 2.0f, ((float)getHeight() - (float)text->getHeight()) / 2.0f, LengthType::PIXEL));
        }

        //visual
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>((float)getWidth(), 0.0f), Point2<float>((float)getWidth(), (float)getHeight()),
                Point2<float>(0.0f, 0.0f), Point2<float>((float)getWidth(), (float)getHeight()), Point2<float>(0.0f, (float)getHeight())
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        buttonRenderer = setupUiRenderer("button", ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(texInfoDefault, TextureParam::buildNearest()))
                ->build();
    }

    const std::shared_ptr<Texture>& Button::getTexture() {
        if (getWidgetState() == FOCUS) {
            return texInfoOnFocus;
        } else if (getWidgetState() == CLICKING) {
            return texInfoOnClick;
        }

        return texInfoDefault;
    }

    bool Button::onKeyPressEvent(unsigned int) {
        buttonRenderer->updateUniformTextureReader(0, TextureReader::build(getTexture(), TextureParam::buildNearest()));
        return true;
    }

    bool Button::onKeyReleaseEvent(unsigned int) {
        buttonRenderer->updateUniformTextureReader(0, TextureReader::build(getTexture(), TextureParam::buildNearest()));
        return true;
    }

    bool Button::onMouseMoveEvent(int, int) {
        buttonRenderer->updateUniformTextureReader(0, TextureReader::build(getTexture(), TextureParam::buildNearest()));
        return true;
    }

    void Button::prepareWidgetRendering(float) {
        updateTranslateVector(buttonRenderer, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        buttonRenderer->addOnRenderTarget();
    }

}
