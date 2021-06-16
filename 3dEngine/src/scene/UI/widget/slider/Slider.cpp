#include <scene/UI/widget/slider/Slider.h>

namespace urchin {

    Slider* Slider::newSlider(Widget* parent, Position position, Size size, std::string nameSkin, const std::vector<std::string>& texts) {
        return new Slider(parent, position, size, std::move(nameSkin), texts);
    }

    Slider::Slider(Widget* parent, Position position, Size size, std::string nameSkin, const std::vector<std::string>& values) :
            Widget(parent, position, size),
            nameSkin(std::move(nameSkin)),
            values(values),
            selectedIndex(0),
            currentValueText(nullptr) {
        if (values.size() < 2) {
            throw std::runtime_error("At least two values must be provided to slider.");
        }
    }

    void Slider::createOrUpdateWidget() {
        //skin information
        std::shared_ptr<XmlChunk> sliderChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "slider", XmlAttribute("nameSkin", nameSkin));

        std::shared_ptr<XmlChunk> valuesTextSkinChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "valuesTextSkin", XmlAttribute(), sliderChunk);
        std::string valuesTextSkin = valuesTextSkinChunk->getStringValue();

        //values
        currentValueText = Text::newText(this, Position(0, 0, LengthType::PIXEL), valuesTextSkin, values[selectedIndex]);
        //TODO center text
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

    void Slider::prepareWidgetRendering(float) {

    }

}
