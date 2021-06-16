#include <scene/UI/widget/slider/Slider.h>
#include <scene/UI/widget/staticbitmap/StaticBitmap.h>
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
        float textYPosition = (getSize().getHeight() - currentValueText->getSize().getHeight()) / 2.0f;
        currentValueText->setPosition(Position(getSize().getWidth() + TEXT_SHIFT_LENGTH, textYPosition, LengthType::PIXEL));

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

    bool Slider::onKeyReleaseEvent(unsigned int) { //TODO improve
        float maxYPosition = std::ceil(getSize().getWidth() - cursorImage->getSize().getWidth());
        float newYPosition = (float)cursorImage->getPositionX() + 1.0f;
        if(newYPosition > maxYPosition) {
            newYPosition = maxYPosition;
        }
        cursorImage->setPosition(Position(newYPosition, (float)cursorImage->getPositionY(), LengthType::PIXEL));
        return false;
    }

    void Slider::prepareWidgetRendering(float) {
        updateTranslateVector(sliderRenderer, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        sliderRenderer->enableRenderer();
    }

}
