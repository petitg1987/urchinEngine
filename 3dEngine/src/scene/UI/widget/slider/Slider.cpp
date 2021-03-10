#include <stdexcept>
#include <utility>

#include "scene/UI/widget/slider/Slider.h"
#include "scene/UI/UISkinService.h"

#define TIME_BEFORE_AUTO_CLICK 0.3
#define TIME_BEFORE_AUTO_NEXT_CLICK 0.09

namespace urchin {

    Slider::Slider(Widget* parent, Position position, Size size, std::string nameSkin, const std::vector<std::string>& values) :
            Widget(parent, position, size),
            values(values),
            selectedIndex(0),
            leftButton(nullptr),
            rightButton(nullptr),
            timeInClickingState(0.0f),
            timeSinceLastChange(0.0f) {
        if (values.empty()) {
            throw std::runtime_error("At least one value must be provided to slider.");
        }

        std::shared_ptr<XmlChunk> sliderChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "slider", XmlAttribute("nameSkin", std::move(nameSkin)));

        std::shared_ptr<XmlChunk> buttonsTextSkinChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "buttonsTextSkin", XmlAttribute(), sliderChunk);
        buttonsTextSkin = buttonsTextSkinChunk->getStringValue();

        std::shared_ptr<XmlChunk> valuesTextSkinChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "valuesTextSkin", XmlAttribute(), sliderChunk);
        valuesTextSkin = valuesTextSkinChunk->getStringValue();

        std::shared_ptr<XmlChunk> leftButtonTextChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "leftButtonText", XmlAttribute(), sliderChunk);
        leftButtonString = leftButtonTextChunk->getStringValue();

        std::shared_ptr<XmlChunk> rightButtonTextChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "rightButtonText", XmlAttribute(), sliderChunk);
        rightButtonString = rightButtonTextChunk->getStringValue();
    }

    Slider::Slider(Position position, Size size, std::string nameSkin, const std::vector<std::string>& values) :
            Slider(nullptr, position, size, std::move(nameSkin), values) {

    }

    void Slider::createOrUpdateWidget() {
        //clear
        delete leftButton;
        delete rightButton;
        for (auto valueText : valuesText) {
            delete valueText;
        }
        valuesText.clear();

        //buttons
        leftButton = new Text(this, Position(0, 0, LengthType::PIXEL), buttonsTextSkin, leftButtonString);
        leftButton->addEventListener(std::make_shared<ButtonSliderEventListener>(this, true));
        if (leftButtonEventListener) {
            this->leftButton->addEventListener(leftButtonEventListener);
        }

        rightButton = new Text(this, Position(0, 0, LengthType::PIXEL), buttonsTextSkin, rightButtonString);
        rightButton->setPosition(Position((float)getWidth() - (float)rightButton->getWidth(), 0.0f, LengthType::PIXEL));
        rightButton->addEventListener(std::make_shared<ButtonSliderEventListener>(this, false));
        if (rightButtonEventListener) {
            this->rightButton->addEventListener(rightButtonEventListener);
        }

        //values
        valuesText.resize(values.size());
        for (std::size_t i = 0; i < values.size(); ++i) {
            auto* valueText = new Text(this, Position(0, 0, LengthType::PIXEL), valuesTextSkin, values[i]);
            valueText->setPosition(Position(((float)getWidth() - (float)valueText->getWidth()) / 2.0f, 0.0f, LengthType::PIXEL));
            valueText->setIsVisible(false);

            valuesText[i] = valueText;
        }
        valuesText[selectedIndex]->setIsVisible(true);
    }

    unsigned int Slider::getSelectedIndex() const {
        return selectedIndex;
    }

    void Slider::setSelectedIndex(unsigned int index) {
        if (index >= values.size()) {
            throw std::out_of_range("Index is out of range: " + std::to_string(index) + ". Maximum index allowed: " + std::to_string(values.size()-1));
        }

        if(!valuesText.empty()) {
            valuesText[selectedIndex]->setIsVisible(false);
            valuesText[index]->setIsVisible(true);
        }

        this->selectedIndex = index;
    }

    void Slider::setLeftButtonEventListener(const std::shared_ptr<EventListener>& leftButtonEventListener) {
        this->leftButtonEventListener = leftButtonEventListener;
        createOrUpdateWidget();
    }

    void Slider::setRightButtonEventListener(const std::shared_ptr<EventListener>& rightButtonEventListener) {
        this->rightButtonEventListener = rightButtonEventListener;
        createOrUpdateWidget();
    }

    void Slider::displayWidget(const ShaderVar&, float dt) {
        if (leftButton->getWidgetState() == Widget::WidgetStates::CLICKING) {
            timeInClickingState += dt;
            timeSinceLastChange += dt;

            if (timeInClickingState > TIME_BEFORE_AUTO_CLICK && timeSinceLastChange > TIME_BEFORE_AUTO_NEXT_CLICK && getSelectedIndex() > 0) {
                setSelectedIndex(getSelectedIndex() - 1);
                timeSinceLastChange = 0.0f;
            }
        } else if (rightButton->getWidgetState() == Widget::WidgetStates::CLICKING) {
            timeInClickingState += dt;
            timeSinceLastChange += dt;

            if (timeInClickingState > TIME_BEFORE_AUTO_CLICK && timeSinceLastChange > TIME_BEFORE_AUTO_NEXT_CLICK && getSelectedIndex() + 1 < values.size()) {
                setSelectedIndex(getSelectedIndex() + 1);
                timeSinceLastChange = 0.0f;
            }
        } else {
            timeInClickingState = 0.0f;
            timeSinceLastChange = 0.0f;
        }
    }

    Slider::ButtonSliderEventListener::ButtonSliderEventListener(Slider* slider, bool isLeftButton) :
            slider(slider),
            isLeftButton(isLeftButton) {

    }

    void Slider::ButtonSliderEventListener::onClickRelease(Widget*) {
        if (isLeftButton) {
            if (slider->selectedIndex > 0) {
                slider->valuesText[slider->selectedIndex]->setIsVisible(false);
                slider->selectedIndex--;
                slider->valuesText[slider->selectedIndex]->setIsVisible(true);
            }
        } else {
            if (slider->selectedIndex + 1 < slider->valuesText.size()) {
                slider->valuesText[slider->selectedIndex]->setIsVisible(false);
                slider->selectedIndex++;
                slider->valuesText[slider->selectedIndex]->setIsVisible(true);
            }
        }
    }

}
