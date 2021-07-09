#include <utility>
#include <UrchinCommon.h>

#include <scene/ui/widget/button/Button.h>
#include <scene/ui/UISkinService.h>
#include <graphic/render/GenericRendererBuilder.h>

namespace urchin {

    Button::Button(Widget* parent, Position position, Size size, std::string nameSkin, std::string buttonText) :
            Widget(parent, position, size),
            nameSkin(std::move(nameSkin)),
            text(nullptr),
            buttonText(std::move(buttonText)) {
        if(parent) {
            Button::createOrUpdateWidget();
        }
    }

    void Button::createOrUpdateWidget() {
        //skin information
        auto buttonChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "button", XmlAttribute("nameSkin", nameSkin));

        auto skinDefaultChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "skin", XmlAttribute("type", "default"), buttonChunk.get());
        texInfoDefault = UISkinService::instance()->createWidgetTexture(getWidth(), getHeight(), skinDefaultChunk.get());
        currentTexture = texInfoDefault;

        auto skinFocusChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "skin", XmlAttribute("type", "focus"), buttonChunk.get());
        texInfoOnFocus = UISkinService::instance()->createWidgetTexture(getWidth(), getHeight(), skinFocusChunk.get());

        auto skinClickChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "skin", XmlAttribute("type", "click"), buttonChunk.get());
        texInfoOnClick = UISkinService::instance()->createWidgetTexture(getWidth(), getHeight(), skinClickChunk.get());

        if (!buttonText.empty()) {
            auto textSkinChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "textSkin", XmlAttribute(), buttonChunk.get());
            delete text;
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

        if(currentTexture != oldTexture) {
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

        updateTranslateVector(buttonRenderer, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        buttonRenderer->enableRenderer();
    }

}
