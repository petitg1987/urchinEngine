#include <scene/ui/widget/slider/Slider.h>
#include <scene/ui/widget/staticbitmap/StaticBitmap.h>
#include <scene/ui/displayer/WidgetInstanceDisplayer.h>
#include <scene/InputDeviceKey.h>
#include <resources/ResourceRetriever.h>

namespace urchin {

    Slider::Slider(Position position, Size size, std::string skinName, const std::vector<ParameterizedText>& values) :
            Widget(position, size),
            skinName(std::move(skinName)),
            values(values),
            selectedIndex(0),
            state(DEFAULT),
            currentValueText(nullptr),
            cursorImage(nullptr) {
        if (values.size() < 2) {
            throw std::runtime_error("At least two values must be provided to slider.");
        }
    }

    std::shared_ptr<Slider> Slider::create(Widget* parent, Position position, Size size, std::string skinName, const std::vector<std::string>& texts) {
        std::vector<ParameterizedText> parameterizedTexts;
        parameterizedTexts.reserve(texts.size());
        for (const std::string& text : texts) {
            parameterizedTexts.emplace_back(text);
        }
        return Widget::create<Slider>(new Slider(position, size, std::move(skinName), parameterizedTexts), parent);
    }

    std::shared_ptr<Slider> Slider::create(Widget* parent, Position position, Size size, std::string skinName, const std::vector<ParameterizedText>& texts) {
        return Widget::create<Slider>(new Slider(position, size, std::move(skinName), texts), parent);
    }

    void Slider::createOrUpdateWidget() {
        //detach children
        detachChild(currentValueText.get());
        detachChild(cursorImage.get());

        //skin information
        auto sliderChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "slider", UdaAttribute("skin", skinName));

        auto valuesTextSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "valuesTextSkin", UdaAttribute(), sliderChunk);
        std::string valuesTextSkin = valuesTextSkinChunk->getStringValue();

        auto cursorImageChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "imageCursor", UdaAttribute(), sliderChunk);
        std::string cursorImageFilename = cursorImageChunk->getStringValue();

        currentValueText = Text::create(this, Position(-1.0f, -1.0f, PIXEL), valuesTextSkin, values[selectedIndex]);
        float textYPosition = (getHeight() - currentValueText->getHeight()) / 2.0f;
        currentValueText->updatePosition(Position(getWidth() + TEXT_SHIFT_LENGTH, textYPosition, PIXEL));

        texSliderLine = loadTexture(sliderChunk, "imageLine");
        changeTexture(texSliderLine);
        auto imageCursor = loadTexture(sliderChunk, "imageCursor");
        float cursorImageWidth = (getHeight() / (float)imageCursor->getHeight()) * (float)imageCursor->getWidth();
        cursorImage = StaticBitmap::create(this, Position(-1.0f, -1.0f, PIXEL), Size(cursorImageWidth, getHeight(), PIXEL), cursorImageFilename);
        moveSliderCursor();
    }

    bool Slider::requireRenderer() const {
        return true;
    }

    WidgetType Slider::getWidgetType() const {
        return WidgetType::SLIDER;
    }

    std::shared_ptr<Texture> Slider::loadTexture(const UdaChunk* sliderChunk, std::string_view chunkName) const {
        auto imageElem = UISkinService::instance().getSkinReader().getFirstChunk(true, chunkName, UdaAttribute(), sliderChunk);
        return ResourceRetriever::instance().getResource<Texture>(imageElem->getStringValue(), {{"mipMap", "0"}});
    }

    unsigned int Slider::getSelectedIndex() const {
        return selectedIndex;
    }

    void Slider::setSelectedIndex(unsigned int index) {
        if (index >= values.size()) {
            throw std::out_of_range("Index is out of range: " + std::to_string(index) + ". Maximum index allowed: " + std::to_string(values.size() - 1));
        }

        this->selectedIndex = index;

        if (currentValueText) {
            currentValueText->updateText(values[selectedIndex]);
            moveSliderCursor();
        }
    }

    bool Slider::onKeyPressEvent(InputDeviceKey key) {
        if (key == InputDeviceKey::MOUSE_LEFT) {
            if (cursorImage->widgetRectangle().collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                state = CURSOR_SELECTED;
                return false;
            } else if (widgetRectangle().collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                updateSliderValue(getMouseX());
                state = CURSOR_SELECTED;
                return false;
            }
        }
        return true;
    }

    bool Slider::onKeyReleaseEvent(InputDeviceKey key) {
        if (key == InputDeviceKey::MOUSE_LEFT) {
            state = DEFAULT;
        }
        return true;
    }

    bool Slider::onMouseMoveEvent(int mouseX, int) {
        if (state == CURSOR_SELECTED) {
            updateSliderValue(mouseX);
            return false;
        }
        return true;
    }

    void Slider::onResetStateEvent() {
        state = DEFAULT;
    }

    void Slider::updateSliderValue(int mouseX) {
        float sliderCursorHalfSize = cursorImage->getWidth() / 2.0f;
        float sliderCursorMinXPosition = sliderCursorHalfSize;
        float sliderCursorMaxXPosition = getWidth() - sliderCursorHalfSize;

        //compute index
        unsigned int oldSelectedIndex = selectedIndex;
        float relativeMouseX = (float)mouseX - getGlobalPositionX();
        float sliderCursorXPosition = std::clamp(relativeMouseX, sliderCursorMinXPosition, sliderCursorMaxXPosition);
        float valuePercentage = 1.0f - ((sliderCursorMaxXPosition - sliderCursorXPosition) / (sliderCursorMaxXPosition - sliderCursorMinXPosition));
        int rawSelectedIndex = MathFunction::floorToInt((float)values.size() * valuePercentage);
        selectedIndex = (unsigned int)std::clamp(rawSelectedIndex, 0, (int)values.size() - 1);

        //move cursor
        moveSliderCursor();

        //update text
        currentValueText->updateText(values[selectedIndex]);

        //event
        if (oldSelectedIndex != selectedIndex) {
            for (auto& eventListener : getEventListeners()) {
                eventListener->onValueChange(this);
            }
        }
    }

    void Slider::moveSliderCursor() const {
        float sliderCursorHalfSize = cursorImage->getWidth() / 2.0f;
        float sliderCursorMinXPosition = sliderCursorHalfSize;
        float sliderCursorMaxXPosition = getWidth() - sliderCursorHalfSize;

        float sliderCursorPositionPercentage = (float)selectedIndex / ((float)values.size() - 1.0f);
        float sliderCursorXPosition = sliderCursorPositionPercentage * (sliderCursorMaxXPosition - sliderCursorMinXPosition);
        cursorImage->updatePosition(Position(sliderCursorXPosition, cursorImage->getPositionY(), LengthType::PIXEL));
    }

}
