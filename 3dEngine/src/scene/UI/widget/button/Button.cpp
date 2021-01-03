#include <utility>
#include "UrchinCommon.h"

#include "scene/UI/widget/button/Button.h"
#include "scene/UI/UISkinService.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    Button::Button(Position position, Size size, std::string nameSkin, std::string buttonText) :
            Widget(position, size),
            nameSkin(std::move(nameSkin)),
            text(nullptr),
            buttonText(std::move(buttonText)) {
        Button::createOrUpdateWidget();
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
            removeChild(text);
            text = new Text(Position(0, 0, Position::PIXEL), textSkinChunk->getStringValue(), Length(16.0f, Length::PIXEL), buttonText); //TODO review font size
            text->setPosition(Position((float)(getWidth() - text->getWidth()) / 2.0f, (float)(getHeight() - text->getHeight()) / 2.0f, Position::PIXEL));
            addChild(text);
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
        buttonRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::TRIANGLE)
                ->addData(&vertexCoord)
                ->addData(&textureCoord)
                ->addTexture(TextureReader::build(texInfoDefault, TextureParam::buildNearest()))
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
        buttonRenderer->updateTexture(0, TextureReader::build(getTexture(), TextureParam::buildNearest()));
        return true;
    }

    bool Button::onKeyReleaseEvent(unsigned int) {
        buttonRenderer->updateTexture(0, TextureReader::build(getTexture(), TextureParam::buildNearest()));
        return true;
    }

    bool Button::onMouseMoveEvent(int, int) {
        buttonRenderer->updateTexture(0, TextureReader::build(getTexture(), TextureParam::buildNearest()));
        return true;
    }

    void Button::display(const RenderTarget* renderTarget, const ShaderVar& translateDistanceShaderVar, float dt) {
        renderTarget->display(buttonRenderer);

        Widget::display(renderTarget, translateDistanceShaderVar, dt);
    }

}
