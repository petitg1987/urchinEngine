#include <stdexcept>
#include <utility>

#include <scene/ui/widget/sequence/Sequence.h>
#include <scene/ui/UISkinService.h>

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
            rightButton(nullptr) {
        if (values.empty()) {
            throw std::runtime_error("At least one value/key must be provided to sequence.");
        }
        if(parent) {
            Sequence::createOrUpdateWidget();
        }
    }

    void Sequence::createOrUpdateWidget() {
        //skin information
        auto sequenceChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "sequence", DataAttribute("nameSkin", nameSkin));

        auto buttonsTextSkinChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "buttonsTextSkin", DataAttribute(), sequenceChunk.get());
        std::string buttonsTextSkin = buttonsTextSkinChunk->getStringValue();

        auto valuesTextSkinChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "valuesTextSkin", DataAttribute(), sequenceChunk.get());
        std::string valuesTextSkin = valuesTextSkinChunk->getStringValue();

        auto leftButtonTextChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "leftButtonText", DataAttribute(), sequenceChunk.get());
        std::string leftButtonString = leftButtonTextChunk->getStringValue();

        auto rightButtonTextChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "rightButtonText", DataAttribute(), sequenceChunk.get());
        std::string rightButtonString = rightButtonTextChunk->getStringValue();

        //clear children
        delete leftButton;
        delete rightButton;
        for (auto valueText : valuesText) {
            delete valueText;
        }
        valuesText.clear();

        //buttons
        leftButton = Text::newText(this, Position(0, 0, LengthType::PIXEL), buttonsTextSkin, leftButtonString);
        leftButton->addEventListener(std::make_shared<ButtonSequenceEventListener>(this, true));
        if (leftButtonEventListener) {
            this->leftButton->addEventListener(leftButtonEventListener);
        }

        rightButton = Text::newText(this, Position(0, 0, LengthType::PIXEL), buttonsTextSkin, rightButtonString);
        rightButton->updatePosition(Position((float)getWidth() - (float)rightButton->getWidth(), 0.0f, LengthType::PIXEL));
        rightButton->addEventListener(std::make_shared<ButtonSequenceEventListener>(this, false));
        if (rightButtonEventListener) {
            this->rightButton->addEventListener(rightButtonEventListener);
        }

        //values
        valuesText.resize(values.size());
        for (std::size_t i = 0; i < values.size(); ++i) {
            if (translatableValues) {
                valuesText[i] = Text::newTranslatableText(this, Position(0, 0, LengthType::PIXEL), valuesTextSkin, values[i]);
            } else {
                valuesText[i] = Text::newText(this, Position(0, 0, LengthType::PIXEL), valuesTextSkin, values[i]);
            }
            valuesText[i]->setIsVisible(false);
        }
        valuesText[selectedIndex]->setIsVisible(true);
    }

    void Sequence::allowLoopOnValues(bool loopOnValuesEnabled) {
        this->loopOnValuesEnabled = loopOnValuesEnabled;
    }

    bool Sequence::isLoopOnValuesAllowed() const {
        return this->loopOnValuesEnabled;
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

    void Sequence::prepareWidgetRendering(float) {
        //update the text position because the text size is updated when the UI language is changed
        valuesText[selectedIndex]->updatePosition(Position(((float)getWidth() - (float)valuesText[selectedIndex]->getWidth()) / 2.0f, 0.0f, LengthType::PIXEL));
    }

    Sequence::ButtonSequenceEventListener::ButtonSequenceEventListener(Sequence* sequence, bool isLeftButton) :
            sequence(sequence),
            isLeftButton(isLeftButton) {

    }

    void Sequence::ButtonSequenceEventListener::onMouseLeftClickRelease(Widget*) {
        unsigned int oldSelectedIndex = sequence->selectedIndex;

        if (isLeftButton) {
            if (sequence->selectedIndex > 0 || sequence->isLoopOnValuesAllowed()) {
                sequence->valuesText[sequence->selectedIndex]->setIsVisible(false);
                if (sequence->selectedIndex == 0) {
                    sequence->selectedIndex = (unsigned int)sequence->valuesText.size() - 1;
                } else {
                    sequence->selectedIndex--;
                }
                sequence->valuesText[sequence->selectedIndex]->setIsVisible(true);
            }
        } else {
            if (sequence->selectedIndex < (sequence->valuesText.size() - 1) || sequence->isLoopOnValuesAllowed()) {
                sequence->valuesText[sequence->selectedIndex]->setIsVisible(false);
                if (sequence->selectedIndex == (sequence->valuesText.size() - 1)) {
                    sequence->selectedIndex = 0;
                } else {
                    sequence->selectedIndex++;
                }
                sequence->valuesText[sequence->selectedIndex]->setIsVisible(true);
            }
        }

        if (oldSelectedIndex != sequence->selectedIndex) {
            for (auto& eventListener : sequence->getEventListeners()) {
                eventListener->onValueChange(sequence);
            }
        }
    }

}
