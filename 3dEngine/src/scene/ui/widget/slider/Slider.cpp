#include <scene/ui/widget/slider/Slider.h>
#include <scene/ui/widget/staticbitmap/StaticBitmap.h>
#include <scene/InputDeviceKey.h>
#include <resources/MediaManager.h>

namespace urchin {

    Slider* Slider::newSlider(Widget* parent, Position position, Size size, std::string nameSkin, const std::vector<std::string>& texts) {
        return new Slider(parent, position, size, std::move(nameSkin), texts);
    }

    Slider::Slider(Widget* parent, Position position, Size size, std::string nameSkin, const std::vector<std::string>& values) :
            Widget(parent, position, size),
            TEXT_SHIFT_LENGTH(10.0f),
            nameSkin(std::move(nameSkin)),
            values(values),
            selectedIndex(0),
            state(DEFAULT),
            currentValueText(nullptr),
            cursorImage(nullptr) {
        if (values.size() < 2) {
            throw std::runtime_error("At least two values must be provided to slider.");
        }
    }

    void Slider::createOrUpdateWidget() {
        //clear children
        delete currentValueText;
        delete cursorImage;

        //skin information
        std::shared_ptr<XmlChunk> sliderChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "slider", XmlAttribute("nameSkin", nameSkin));

        std::shared_ptr<XmlChunk> valuesTextSkinChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "valuesTextSkin", XmlAttribute(), sliderChunk);
        std::string valuesTextSkin = valuesTextSkinChunk->getStringValue();

        std::shared_ptr<XmlChunk> cursorImageElem = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "imageCursor", XmlAttribute(), sliderChunk);
        std::string cursorImageFilename = cursorImageElem->getStringValue();

        currentValueText = Text::newText(this, Position(0, 0, LengthType::PIXEL), valuesTextSkin, values[selectedIndex]);
        float textYPosition = (float)(getHeight() - currentValueText->getHeight()) / 2.0f;
        currentValueText->setPosition(Position((float)getWidth() + TEXT_SHIFT_LENGTH, textYPosition, LengthType::PIXEL));

        texSliderLine = loadTexture(sliderChunk, "imageLine");
        auto imageCursor = loadTexture(sliderChunk, "imageCursor");
        float cursorImageWidth = ((float)getHeight() / (float)imageCursor->getHeight()) * (float)imageCursor->getWidth();
        imageCursor.reset();
        cursorImage = new StaticBitmap(this, Position(0, 0, LengthType::PIXEL), Size((float)cursorImageWidth, (float)getHeight(), LengthType::PIXEL), cursorImageFilename);

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
                ->enableTransparency()
                ->build();
    }

    std::shared_ptr<Texture> Slider::loadTexture(const std::shared_ptr<XmlChunk>& sliderChunk, const std::string& chunkName) const {
        std::shared_ptr<XmlChunk> imageElem = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, chunkName, XmlAttribute(), sliderChunk);

        auto* img = MediaManager::instance()->getMedia<Image>(imageElem->getStringValue());
        auto tex = img->createTexture(false);
        img->release();
        return tex;
    }

    unsigned int Slider::getSelectedIndex() const {
        return selectedIndex;
    }

    void Slider::setSelectedIndex(unsigned int index) {
        if (index >= values.size()) {
            throw std::out_of_range("Index is out of range: " + std::to_string(index) + ". Maximum index allowed: " + std::to_string(values.size() - 1));
        }

        if (currentValueText) {
            currentValueText->updateText(values[index]);
        }

        this->selectedIndex = index;
    }

    bool Slider::onKeyPressEvent(unsigned int key) {
        if (key == InputDeviceKey::MOUSE_LEFT) {
            Rectangle<int> cursorRectangle(Point2<int>(cursorImage->getGlobalPositionX(), cursorImage->getGlobalPositionY()),
                    Point2<int>(cursorImage->getGlobalPositionX() + (int)cursorImage->getWidth(), cursorImage->getGlobalPositionY() + (int)cursorImage->getHeight()));
            if (cursorRectangle.collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                state = CURSOR_SELECTED;
            }
        }
        return true;
    }

    bool Slider::onKeyReleaseEvent(unsigned int key) {
        if (key == InputDeviceKey::MOUSE_LEFT) {
            state = DEFAULT;
        }
        return true;
    }

    bool Slider::onMouseMoveEvent(int mouseX, int) {
        if (state == CURSOR_SELECTED) {
            float sliderCursorHalfSize = (float)cursorImage->getWidth() / 2.0f;
            float sliderCursorMinXPosition = sliderCursorHalfSize;
            float sliderCursorMaxXPosition = ((float)getWidth() - (float)sliderCursorHalfSize);

            //move cursor
            float relativeMouseX = (float)mouseX - (float)getGlobalPositionX();
            float sliderCursorCenterXPosition = MathFunction::clamp(relativeMouseX, sliderCursorMinXPosition, sliderCursorMaxXPosition) - sliderCursorMinXPosition;
            cursorImage->setPosition(Position((float)sliderCursorCenterXPosition, (float)cursorImage->getPositionY(), LengthType::PIXEL));

            //compute index
            unsigned int oldSelectedIndex = selectedIndex;
            float sliderCursorCurrentPosition = (float)cursorImage->getPositionX() + sliderCursorHalfSize;
            float valuePercentage = 1.0f - ((sliderCursorMaxXPosition - sliderCursorCurrentPosition) / (sliderCursorMaxXPosition - sliderCursorMinXPosition));
            selectedIndex = MathFunction::floorToUInt((float)values.size() * valuePercentage);
            selectedIndex = MathFunction::clamp(selectedIndex, 0u, (unsigned int)values.size() - 1u);

            //update text
            currentValueText->updateText(values[selectedIndex]);

            //event
            if (oldSelectedIndex != selectedIndex) {
                for (auto& eventListener : getEventListeners()) {
                    eventListener->onValueChange(this);
                }
            }

            return false;
        }
        return true;
    }

    void Slider::prepareWidgetRendering(float) {
        updateTranslateVector(sliderRenderer, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        sliderRenderer->enableRenderer();
    }

}
