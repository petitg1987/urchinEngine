#include <utility>
#include <UrchinCommon.h>

#include <scene/ui/widget/button/Button.h>
#include <scene/ui/UISkinService.h>
#include <graphic/render/GenericRendererBuilder.h>

namespace urchin {

    Button::Button(Position position, Size size, std::string nameSkin, std::string buttonText) :
            Widget(position, size),
            nameSkin(std::move(nameSkin)),
            text(nullptr),
            buttonText(std::move(buttonText)) {

    }

    std::shared_ptr<Button> Button::newButton(Widget* parent, Position position, Size size, std::string nameSkin, std::string buttonText) {
        auto widget = std::shared_ptr<Button>(new Button(position, size, std::move(nameSkin), std::move(buttonText)));
        if(parent) {
            parent->addChild(widget);
        }
        return widget;
    }

    void Button::createOrUpdateWidget() {
        //skin information
        auto buttonChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "button", UdaAttribute("nameSkin", nameSkin));

        auto skinDefaultChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "skin", UdaAttribute("type", "default"), buttonChunk);
        texInfoDefault = UISkinService::instance()->createWidgetTexture(getWidth(), getHeight(), skinDefaultChunk);
        currentTexture = texInfoDefault;

        auto skinFocusChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "skin", UdaAttribute("type", "focus"), buttonChunk);
        texInfoOnFocus = UISkinService::instance()->createWidgetTexture(getWidth(), getHeight(), skinFocusChunk);

        auto skinClickChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "skin", UdaAttribute("type", "click"), buttonChunk);
        texInfoOnClick = UISkinService::instance()->createWidgetTexture(getWidth(), getHeight(), skinClickChunk);

        if (!buttonText.empty()) {
            auto textSkinChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "textSkin", UdaAttribute(), buttonChunk);
            text = Text::newTranslatableText(this, Position(0, 0, LengthType::PIXEL), textSkinChunk->getStringValue(), buttonText);
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
                ->addUniformTextureReader(TextureReader::build(currentTexture, TextureParam::buildLinear())) //binding 2
                ->build();
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
            buttonRenderer->updateUniformTextureReader(0, TextureReader::build(currentTexture, TextureParam::buildLinear()));
        }
    }

    bool Button::onKeyPressEvent(unsigned int) {
        refreshTexture();
        return true;
    }

    bool Button::onKeyReleaseEvent(unsigned int) {
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
            text->updatePosition(Position(((float)getWidth() - (float)text->getWidth()) / 2.0f, ((float)getHeight() - (float)text->getHeight()) / 2.0f, LengthType::PIXEL));
        }

        updateTranslateVector(buttonRenderer.get(), Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        buttonRenderer->enableRenderer();
    }

}
