#include <utility>
#include <UrchinCommon.h>

#include <scene/ui/widget/button/Button.h>
#include <scene/ui/UISkinService.h>
#include <api/render/GenericRendererBuilder.h>

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
        auto buttonChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "button", UdaAttribute("skin", skinName));

        auto skinDefaultChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "skin", UdaAttribute("type", "default"), buttonChunk);
        texInfoDefault = UISkinService::instance().createWidgetTexture(getWidth(), getHeight(), skinDefaultChunk);
        currentTexture = texInfoDefault;

        auto skinFocusChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "skin", UdaAttribute("type", "focus"), buttonChunk);
        texInfoOnFocus = UISkinService::instance().createWidgetTexture(getWidth(), getHeight(), skinFocusChunk);

        auto skinClickChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "skin", UdaAttribute("type", "click"), buttonChunk);
        texInfoOnClick = UISkinService::instance().createWidgetTexture(getWidth(), getHeight(), skinClickChunk);

        if (!buttonText.empty()) {
            auto textSkinChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "textSkin", UdaAttribute(), buttonChunk);
            text = Text::createTranslatable(this, Position(0.0f, 0.0f, LengthType::PIXEL), textSkinChunk->getStringValue(), buttonText);
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

    void Button::prepareWidgetRendering(float, unsigned int& renderingOrder, const Matrix4<float>& viewModelMatrix) {
        if (text) {
            //update the text position because the text size is updated when the UI language is changed
            text->updatePosition(Position(((float)getWidth() - (float)text->getWidth()) / 2.0f, ((float)getHeight() - (float)text->getHeight()) / 2.0f, LengthType::PIXEL));
        }

        updatePositioning(buttonRenderer.get(), viewModelMatrix, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        buttonRenderer->enableRenderer(renderingOrder);
    }

}
