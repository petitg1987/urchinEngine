#include <stdexcept>

#include "scene/GUI/widget/slider/Slider.h"
#include "scene/GUI/GUISkinService.h"

#define TIME_BEFORE_AUTO_CLICK 0.3
#define TIME_BEFORE_AUTO_NEXT_CLICK 0.09

namespace urchin {

    Slider::Slider(Position position, Size size, const std::vector<std::string> &values, const std::string &nameSkin) :
        Widget(position, size),
        values(values),
        selectedIndex(0),
        leftButton(nullptr),
        rightButton(nullptr),
        timeInClickingState(0.0f),
        timeSinceLastChange(0.0f) {
        if(values.empty()) {
            throw std::runtime_error("At least one value must be provided to slider.");
        }

        std::shared_ptr<XmlChunk> sliderChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "slider", XmlAttribute("nameSkin", nameSkin));

        std::shared_ptr<XmlChunk> buttonsTextFontChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "buttonTextFont", XmlAttribute(), sliderChunk);
        buttonNameFont = buttonsTextFontChunk->getStringValue();

        std::shared_ptr<XmlChunk> valuesTextFontChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "valuesTextFont", XmlAttribute(), sliderChunk);
        valuesNameFont = valuesTextFontChunk->getStringValue();

        std::shared_ptr<XmlChunk> leftButtonTextChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "leftButtonText", XmlAttribute(), sliderChunk);
        leftButtonString = leftButtonTextChunk->getStringValue();

        std::shared_ptr<XmlChunk> rightButtonTextChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "rightButtonText", XmlAttribute(), sliderChunk);
        rightButtonString = rightButtonTextChunk->getStringValue();

        Slider::createOrUpdateWidget();
    }

    void Slider::createOrUpdateWidget() {
        //clear
        Widget::removeChild(leftButton);
        Widget::removeChild(rightButton);
        for (auto &valueText : valuesText) {
            Widget::removeChild(valueText);
        }
        valuesText.clear();

        //buttons
        leftButton = new Text(Position(0, 0, Position::PIXEL), buttonNameFont);
        leftButton->setText(leftButtonString);
        leftButton->addEventListener(std::make_shared<ButtonSliderEventListener>(this, true));
        if(leftButtonEventListener) {
            this->leftButton->addEventListener(leftButtonEventListener);
        }
        Widget::addChild(leftButton);

        rightButton = new Text(Position(0, 0, Position::PIXEL), buttonNameFont);
        rightButton->setText(rightButtonString);
        rightButton->setPosition(Position((float)(getWidth()-rightButton->getWidth()), 0, Position::PIXEL));
        rightButton->addEventListener(std::make_shared<ButtonSliderEventListener>(this, false));
        if(rightButtonEventListener) {
            this->rightButton->addEventListener(rightButtonEventListener);
        }
        Widget::addChild(rightButton);

        //values
        valuesText.resize(values.size());
        for(std::size_t i=0; i<values.size(); ++i) {
            Text *valueText = new Text(Position(0, 0, Position::PIXEL), valuesNameFont);
            valueText->setText(values[i]);
            valueText->setPosition(Position((float)(getWidth()-valueText->getWidth()) / 2.0f, 0, Position::PIXEL));
            valueText->setIsVisible(false);

            Widget::addChild(valueText);
            valuesText[i] = valueText;
        }
        valuesText[selectedIndex]->setIsVisible(true);
    }

    unsigned int Slider::getSelectedIndex() const {
        return selectedIndex;
    }

    void Slider::setSelectedIndex(unsigned int index) {
        if(index >= values.size()) {
            throw std::out_of_range("Index is out of range: " + std::to_string(index) + ". Maximum index allowed: " + std::to_string(values.size()-1));
        }

        valuesText[selectedIndex]->setIsVisible(false);
        valuesText[index]->setIsVisible(true);

        this->selectedIndex = index;
    }

    void Slider::setLeftButtonEventListener(const std::shared_ptr<EventListener> &leftButtonEventListener) {
        this->leftButtonEventListener = leftButtonEventListener;
        this->leftButton->addEventListener(leftButtonEventListener);
    }

    void Slider::setRightButtonEventListener(const std::shared_ptr<EventListener> &rightButtonEventListener) {
        this->rightButtonEventListener = rightButtonEventListener;
        this->rightButton->addEventListener(rightButtonEventListener);
    }

    void Slider::display(int translateDistanceLoc, float dt) {
        if(leftButton->getWidgetState()==Widget::WidgetStates::CLICKING) {
            timeInClickingState += dt;
            timeSinceLastChange += dt;

            if(timeInClickingState>TIME_BEFORE_AUTO_CLICK && timeSinceLastChange>TIME_BEFORE_AUTO_NEXT_CLICK
                    && getSelectedIndex() > 0) {
                setSelectedIndex(getSelectedIndex()-1);
                timeSinceLastChange = 0.0f;
            }
        } else if(rightButton->getWidgetState()==Widget::WidgetStates::CLICKING) {
            timeInClickingState += dt;
            timeSinceLastChange += dt;

            if(timeInClickingState>TIME_BEFORE_AUTO_CLICK && timeSinceLastChange>TIME_BEFORE_AUTO_NEXT_CLICK
                    && getSelectedIndex()+1 < values.size()) {
                setSelectedIndex(getSelectedIndex()+1);
                timeSinceLastChange = 0.0f;
            }
        } else {
            timeInClickingState = 0.0f;
            timeSinceLastChange = 0.0f;
        }

        Widget::display(translateDistanceLoc, dt);
    }

    Slider::ButtonSliderEventListener::ButtonSliderEventListener(Slider *slider, bool isLeftButton) :
            slider(slider),
            isLeftButton(isLeftButton) {

    }

    void Slider::ButtonSliderEventListener::onClickRelease(Widget *) {
        if(isLeftButton) {
            if(slider->selectedIndex > 0) {
                slider->valuesText[slider->selectedIndex]->setIsVisible(false);
                slider->selectedIndex--;
                slider->valuesText[slider->selectedIndex]->setIsVisible(true);
            }
        } else {
            if(slider->selectedIndex+1 < slider->valuesText.size()) {
                slider->valuesText[slider->selectedIndex]->setIsVisible(false);
                slider->selectedIndex++;
                slider->valuesText[slider->selectedIndex]->setIsVisible(true);
            }
        }
    }

}
