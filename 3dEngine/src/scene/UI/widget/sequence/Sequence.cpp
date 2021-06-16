#include <stdexcept>
#include <utility>

#include <scene/UI/widget/sequence/Sequence.h>
#include <scene/UI/UISkinService.h>

#define TIME_BEFORE_AUTO_CLICK 0.3
#define TIME_BEFORE_AUTO_NEXT_CLICK 0.15

namespace urchin {

    Sequence* Sequence::newSequence(Widget* parent, Position position, Size size, std::string nameSkin, const std::vector<std::string>& texts) {
        return new Sequence(parent, position, size, std::move(nameSkin), texts, false);
    }

    Sequence* Sequence::newTranslatableSequence(Widget* parent, Position position, Size size, std::string nameSkin, const std::vector<std::string>& textKeys) {
        return new Sequence(parent, position, size, std::move(nameSkin), textKeys, true);
    }

    Sequence::Sequence(Widget* parent, Position position, Size size, std::string nameSkin, const std::vector<std::string>& values, bool translatableValues) :
            Widget(parent, position, size),
            nameSkin(std::move(nameSkin)),
            values(values),
            translatableValues(translatableValues),
            loopOnValuesEnabled(true),
            selectedIndex(0),
            leftButton(nullptr),
            rightButton(nullptr),
            timeInClickingState(0.0f),
            timeSinceLastChange(0.0f) {
        if (values.empty()) {
            throw std::runtime_error("At least one value/key must be provided to sequence.");
        }
    }

    void Sequence::createOrUpdateWidget() {
        //skin information
        std::shared_ptr<XmlChunk> sequenceChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "sequence", XmlAttribute("nameSkin", nameSkin));

        std::shared_ptr<XmlChunk> buttonsTextSkinChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "buttonsTextSkin", XmlAttribute(), sequenceChunk);
        std::string buttonsTextSkin = buttonsTextSkinChunk->getStringValue();

        std::shared_ptr<XmlChunk> valuesTextSkinChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "valuesTextSkin", XmlAttribute(), sequenceChunk);
        std::string valuesTextSkin = valuesTextSkinChunk->getStringValue();

        std::shared_ptr<XmlChunk> leftButtonTextChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "leftButtonText", XmlAttribute(), sequenceChunk);
        std::string leftButtonString = leftButtonTextChunk->getStringValue();

        std::shared_ptr<XmlChunk> rightButtonTextChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "rightButtonText", XmlAttribute(), sequenceChunk);
        std::string rightButtonString = rightButtonTextChunk->getStringValue();

        //clear
        delete leftButton;
        delete rightButton;
        for (auto valueText : valuesText) {
            delete valueText;
        }
        valuesText.clear();

        //buttons
        leftButton = Text::newText(this, Position(0, 0, LengthType::PIXEL), buttonsTextSkin, leftButtonString);
        leftButton->addEventListener(std::make_shared<ButtonSequenceEventListener>(this, true, loopOnValuesEnabled));
        if (leftButtonEventListener) {
            this->leftButton->addEventListener(leftButtonEventListener);
        }

        rightButton = Text::newText(this, Position(0, 0, LengthType::PIXEL), buttonsTextSkin, rightButtonString);
        rightButton->setPosition(Position((float)getWidth() - (float)rightButton->getWidth(), 0.0f, LengthType::PIXEL));
        rightButton->addEventListener(std::make_shared<ButtonSequenceEventListener>(this, false, loopOnValuesEnabled));
        if (rightButtonEventListener) {
            this->rightButton->addEventListener(rightButtonEventListener);
        }

        //values
        valuesText.resize(values.size());
        for (std::size_t i = 0; i < values.size(); ++i) {
            Text* valueText;
            if (translatableValues) {
                valueText = Text::newTranslatableText(this, Position(0, 0, LengthType::PIXEL), valuesTextSkin, values[i]);
            } else {
                valueText = Text::newText(this, Position(0, 0, LengthType::PIXEL), valuesTextSkin, values[i]);
            }
            valueText->setIsVisible(false);

            valuesText[i] = valueText;
        }
        valuesText[selectedIndex]->setIsVisible(true);
    }

    void Sequence::allowLoopOnValues(bool loopOnValuesEnabled) {
        this->loopOnValuesEnabled = loopOnValuesEnabled;
    }

    unsigned int Sequence::getSelectedIndex() const {
        return selectedIndex;
    }

    void Sequence::setSelectedIndex(unsigned int index) {
        if (index >= values.size()) {
            throw std::out_of_range("Index is out of range: " + std::to_string(index) + ". Maximum index allowed: " + std::to_string(values.size() - 1));
        }

        if (!valuesText.empty()) {
            valuesText[selectedIndex]->setIsVisible(false);
            valuesText[index]->setIsVisible(true);
        }

        this->selectedIndex = index;
    }

    void Sequence::setLeftButtonEventListener(const std::shared_ptr<EventListener>& leftButtonEventListener) {
        this->leftButtonEventListener = leftButtonEventListener;
        createOrUpdateWidget();
    }

    void Sequence::setRightButtonEventListener(const std::shared_ptr<EventListener>& rightButtonEventListener) {
        this->rightButtonEventListener = rightButtonEventListener;
        createOrUpdateWidget();
    }

    void Sequence::prepareWidgetRendering(float dt) {
        if (leftButton->getWidgetState() == Widget::WidgetStates::CLICKING) {
            timeInClickingState += dt;
            timeSinceLastChange += dt;

            if (timeInClickingState > TIME_BEFORE_AUTO_CLICK && timeSinceLastChange > TIME_BEFORE_AUTO_NEXT_CLICK &&
                    (getSelectedIndex() > 0 || loopOnValuesEnabled)) {
                if(getSelectedIndex() == 0) {
                    setSelectedIndex((unsigned int)values.size() - 1);
                } else {
                    setSelectedIndex(getSelectedIndex() - 1);
                }
                timeSinceLastChange = 0.0f;
            }
        } else if (rightButton->getWidgetState() == Widget::WidgetStates::CLICKING) {
            timeInClickingState += dt;
            timeSinceLastChange += dt;

            if (timeInClickingState > TIME_BEFORE_AUTO_CLICK && timeSinceLastChange > TIME_BEFORE_AUTO_NEXT_CLICK &&
                    (getSelectedIndex() + 1 < values.size() || loopOnValuesEnabled)) {
                if (getSelectedIndex() == (values.size() - 1)) {
                    setSelectedIndex(0);
                } else {
                    setSelectedIndex(getSelectedIndex() + 1);
                }
                timeSinceLastChange = 0.0f;
            }
        } else {
            timeInClickingState = 0.0f;
            timeSinceLastChange = 0.0f;
        }

        //update the text position because the text size is updated when the UI language is changed
        valuesText[selectedIndex]->setPosition(Position(((float)getWidth() - (float)valuesText[selectedIndex]->getWidth()) / 2.0f, 0.0f, LengthType::PIXEL));
    }

    Sequence::ButtonSequenceEventListener::ButtonSequenceEventListener(Sequence* sequence, bool isLeftButton, bool loopOnValuesEnabled) :
            sequence(sequence),
            isLeftButton(isLeftButton),
            loopOnValuesEnabled(loopOnValuesEnabled) {

    }

    void Sequence::ButtonSequenceEventListener::onMouseLeftClickRelease(Widget*) {
        if (isLeftButton) {
            if (sequence->selectedIndex > 0 || loopOnValuesEnabled) {
                sequence->valuesText[sequence->selectedIndex]->setIsVisible(false);
                if (sequence->selectedIndex == 0) {
                    sequence->selectedIndex = (unsigned int)sequence->valuesText.size() - 1;
                } else {
                    sequence->selectedIndex--;
                }
                sequence->valuesText[sequence->selectedIndex]->setIsVisible(true);
            }
        } else {
            if (sequence->selectedIndex < (sequence->valuesText.size() - 1) || loopOnValuesEnabled) {
                sequence->valuesText[sequence->selectedIndex]->setIsVisible(false);
                if (sequence->selectedIndex == (sequence->valuesText.size() - 1)) {
                    sequence->selectedIndex = 0;
                } else {
                    sequence->selectedIndex++;
                }
                sequence->valuesText[sequence->selectedIndex]->setIsVisible(true);
            }
        }
    }

}
