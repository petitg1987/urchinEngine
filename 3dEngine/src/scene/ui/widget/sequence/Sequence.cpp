#include <stdexcept>
#include <utility>

#include <scene/ui/widget/sequence/Sequence.h>
#include <scene/ui/UISkinService.h>

namespace urchin {

    Sequence::Sequence(Position position, Size size, std::string skinName, const std::vector<std::string>& values, bool translatableValues) :
            Widget(position, size),
            skinName(std::move(skinName)),
            values(values),
            translatableValues(translatableValues),
            loopOnValuesEnabled(true),
            selectedIndex(0),
            leftButton(nullptr),
            rightButton(nullptr) {
        if (values.empty()) {
            throw std::runtime_error("At least one value/key must be provided to sequence.");
        }
    }

    std::shared_ptr<Sequence> Sequence::newSequence(Widget* parent, Position position, Size size, std::string skinName, const std::vector<std::string>& texts) {
        return create<Sequence>(new Sequence(position, size, std::move(skinName), texts, false), parent);
    }

    std::shared_ptr<Sequence> Sequence::newTranslatableSequence(Widget* parent, Position position, Size size, std::string skinName, const std::vector<std::string>& textKeys) {
        return create<Sequence>(new Sequence(position, size, std::move(skinName), textKeys, true), parent);
    }

    void Sequence::createOrUpdateWidget() {
        //detach children
        for(auto &valueText : valuesText) {
            detachChild(valueText.get());
        }
        detachChild(rightButton.get());
        detachChild(leftButton.get());

        //skin information
        auto sequenceChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "sequence", UdaAttribute("skin", skinName));

        auto buttonsTextSkinChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "buttonsTextSkin", UdaAttribute(), sequenceChunk);
        std::string buttonsTextSkin = buttonsTextSkinChunk->getStringValue();

        auto valuesTextSkinChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "valuesTextSkin", UdaAttribute(), sequenceChunk);
        std::string valuesTextSkin = valuesTextSkinChunk->getStringValue();

        auto leftButtonTextChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "leftButtonText", UdaAttribute(), sequenceChunk);
        std::string leftButtonString = leftButtonTextChunk->getStringValue();

        auto rightButtonTextChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "rightButtonText", UdaAttribute(), sequenceChunk);
        std::string rightButtonString = rightButtonTextChunk->getStringValue();

        //buttons
        leftButton = Text::newText(this, Position(0, 0, LengthType::PIXEL), buttonsTextSkin, leftButtonString);
        leftButton->addEventListener(std::make_unique<ButtonSequenceEventListener>(this, true));
        if (leftButtonEventListener) {
            this->leftButton->addEventListener(leftButtonEventListener);
        }

        rightButton = Text::newText(this, Position(0, 0, LengthType::PIXEL), buttonsTextSkin, rightButtonString);
        rightButton->addEventListener(std::make_unique<ButtonSequenceEventListener>(this, false));
        if (rightButtonEventListener) {
            this->rightButton->addEventListener(rightButtonEventListener);
        }
        rightButton->updatePosition(Position((float)getWidth() - (float)rightButton->getWidth(), 0.0f, LengthType::PIXEL));

        //values
        valuesText.resize(values.size(), std::shared_ptr<Text>(nullptr));
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

    void Sequence::setLeftButtonEventListener(std::shared_ptr<EventListener> leftButtonEventListener) {
        this->leftButtonEventListener = std::move(leftButtonEventListener);
        createOrUpdateWidget();
    }

    void Sequence::setRightButtonEventListener(std::shared_ptr<EventListener> rightButtonEventListener) {
        this->rightButtonEventListener = std::move(rightButtonEventListener);
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
