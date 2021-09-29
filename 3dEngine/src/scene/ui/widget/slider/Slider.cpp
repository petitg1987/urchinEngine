#include <scene/ui/widget/slider/Slider.h>
#include <scene/ui/widget/staticbitmap/StaticBitmap.h>
#include <scene/InputDeviceKey.h>
#include <resources/ResourceRetriever.h>

namespace urchin {

    Slider::Slider(Position position, Size size, std::string skinName, const std::vector<std::string>& values, bool translatableValues) :
            Widget(position, size),
            TEXT_SHIFT_LENGTH(10.0f),
            skinName(std::move(skinName)),
            values(values),
            translatableValues(translatableValues),
            selectedIndex(0),
            state(DEFAULT),
            currentValueText(nullptr),
            cursorImage(nullptr) {
        if (values.size() < 2) {
            throw std::runtime_error("At least two values must be provided to slider.");
        }
    }

    std::shared_ptr<Slider> Slider::create(Widget* parent, Position position, Size size, std::string skinName, const std::vector<std::string>& texts) {
        return Widget::create<Slider>(new Slider(position, size, std::move(skinName), texts, false), parent);
    }

    std::shared_ptr<Slider> Slider::createTranslatable(Widget* parent, Position position, Size size, std::string skinName, const std::vector<std::string>& textKeys) {
        return Widget::create<Slider>(new Slider(position, size, std::move(skinName), textKeys, true), parent);
    }

    void Slider::createOrUpdateWidget() {
        //detach children
        detachChild(currentValueText.get());
        detachChild(cursorImage.get());

        //skin information
        auto sliderChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "slider", UdaAttribute("skin", skinName));

        auto valuesTextSkinChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "valuesTextSkin", UdaAttribute(), sliderChunk);
        std::string valuesTextSkin = valuesTextSkinChunk->getStringValue();

        auto cursorImageChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "imageCursor", UdaAttribute(), sliderChunk);
        std::string cursorImageFilename = cursorImageChunk->getStringValue();

        if (translatableValues) {
            currentValueText = Text::createTranslatable(this, Position(0.0f, 0.0f, LengthType::PIXEL), valuesTextSkin, values[selectedIndex]);
        } else {
            currentValueText = Text::create(this, Position(0.0f, 0.0f, LengthType::PIXEL), valuesTextSkin, values[selectedIndex]);
        }
        float textYPosition = (float)(getHeight() - currentValueText->getHeight()) / 2.0f;
        currentValueText->updatePosition(Position((float)getWidth() + TEXT_SHIFT_LENGTH, textYPosition, LengthType::PIXEL));

        texSliderLine = loadTexture(sliderChunk, "imageLine");
        auto imageCursor = loadTexture(sliderChunk, "imageCursor");
        float cursorImageWidth = ((float)getHeight() / (float)imageCursor->getHeight()) * (float)imageCursor->getWidth();
        cursorImage = StaticBitmap::create(this, Position(0.0f, 0.0f, LengthType::PIXEL), Size((float)cursorImageWidth, (float)getHeight(), LengthType::PIXEL), cursorImageFilename);
        moveSliderCursor();

        //visual
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>((float)getWidth(), 0.0f), Point2<float>((float)getWidth(), (float)getHeight()),
                Point2<float>(0.0f, 0.0f), Point2<float>((float)getWidth(), (float)getHeight()), Point2<float>(0.0f, (float)getHeight())
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        sliderRenderer = setupUiRenderer("slider", ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(texSliderLine, TextureParam::buildLinear())) //binding 3
                ->enableTransparency({BlendFunction::buildDefault()})
                ->build();
    }

    std::shared_ptr<Texture> Slider::loadTexture(const UdaChunk* sliderChunk, const std::string& chunkName) const {
        auto imageElem = UISkinService::instance().getSkinReader().getUniqueChunk(true, chunkName, UdaAttribute(), sliderChunk);
        auto img = ResourceRetriever::instance().getResource<Image>(imageElem->getStringValue());
        return img->createTexture(false);
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
            if (translatableValues) {
                currentValueText->updateLabelKey(values[selectedIndex]);
            } else {
                currentValueText->updateText(values[selectedIndex]);
            }
            moveSliderCursor();
        }
    }

    bool Slider::onKeyPressEvent(unsigned int key) {
        if (key == (int)InputDeviceKey::MOUSE_LEFT) {
            if (cursorImage->widgetRectangle().collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                state = CURSOR_SELECTED;
            } else if (widgetRectangle().collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                updateSliderValue(getMouseX());
                state = CURSOR_SELECTED;
            }
        }
        return true;
    }

    bool Slider::onKeyReleaseEvent(unsigned int key) {
        if (key == (int)InputDeviceKey::MOUSE_LEFT) {
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

    void Slider::updateSliderValue(int mouseX) {
        float sliderCursorHalfSize = (float)cursorImage->getWidth() / 2.0f;
        float sliderCursorMinXPosition = sliderCursorHalfSize;
        float sliderCursorMaxXPosition = (float)getWidth() - sliderCursorHalfSize;

        //compute index
        unsigned int oldSelectedIndex = selectedIndex;
        float relativeMouseX = (float)mouseX - (float)getGlobalPositionX();
        float sliderCursorXPosition = MathFunction::clamp(relativeMouseX, sliderCursorMinXPosition, sliderCursorMaxXPosition);
        float valuePercentage = 1.0f - ((sliderCursorMaxXPosition - sliderCursorXPosition) / (sliderCursorMaxXPosition - sliderCursorMinXPosition));
        selectedIndex = MathFunction::floorToUInt((float)values.size() * valuePercentage);
        selectedIndex = MathFunction::clamp(selectedIndex, 0u, (unsigned int)values.size() - 1u);

        //move cursor
        moveSliderCursor();

        //update text
        if (translatableValues) {
            currentValueText->updateLabelKey(values[selectedIndex]);
        } else {
            currentValueText->updateText(values[selectedIndex]);
        }

        //event
        if (oldSelectedIndex != selectedIndex) {
            for (auto& eventListener : getEventListeners()) {
                eventListener->onValueChange(this);
            }
        }
    }

    void Slider::moveSliderCursor() {
        float sliderCursorHalfSize = (float)cursorImage->getWidth() / 2.0f;
        float sliderCursorMinXPosition = sliderCursorHalfSize;
        float sliderCursorMaxXPosition = ((float)getWidth() - (float)sliderCursorHalfSize);

        float sliderCursorPositionPercentage = (float)selectedIndex / ((float)values.size() - 1.0f);
        float sliderCursorXPosition = sliderCursorPositionPercentage * (sliderCursorMaxXPosition - sliderCursorMinXPosition);
        cursorImage->updatePosition(Position((float)sliderCursorXPosition, (float)cursorImage->getPositionY(), LengthType::PIXEL));
    }

    void Slider::prepareWidgetRendering(float, unsigned int& renderingOrder) {
        updateTranslateVector(sliderRenderer.get(), Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        sliderRenderer->enableRenderer(renderingOrder);
    }

}
