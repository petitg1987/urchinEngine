#include <scene/UI/widget/slider/Slider.h>

namespace urchin {

    Slider* Slider::newSlider(Widget* parent, Position position, Size size, std::string nameSkin, const std::vector<std::string>& texts) {
        return new Slider(parent, position, size, std::move(nameSkin), texts);
    }

    Slider::Slider(Widget* parent, Position position, Size size, std::string nameSkin, const std::vector<std::string>& values) :
            Widget(parent, position, size),
            values(values),
            selectedIndex(0) {
        if (values.empty()) {
            throw std::runtime_error("At least one value must be provided to slider.");
        }

        std::shared_ptr<XmlChunk> sliderChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "slider", XmlAttribute("nameSkin", std::move(nameSkin)));

        //TODO complete...
    }

    void Slider::createOrUpdateWidget() {

    }

    void Slider::prepareWidgetRendering(float) {

    }

}
