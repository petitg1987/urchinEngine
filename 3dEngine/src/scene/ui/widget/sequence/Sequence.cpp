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

    std::shared_ptr<Sequence> Sequence::create(Widget* parent, Position position, Size size, std::string skinName, const std::vector<std::string>& texts) {
        return Widget::create<Sequence>(new Sequence(position, size, std::move(skinName), texts, false), parent);
    }

    std::shared_ptr<Sequence> Sequence::createTranslatable(Widget* parent, Position position, Size size, std::string skinName, const std::vector<std::string>& textKeys) {
        return Widget::create<Sequence>(new Sequence(position, size, std::move(skinName), textKeys, true), parent);
    }

    void Sequence::createOrUpdateWidget() {
        //detach children
        for (const auto& valueText : valuesText) {
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
        leftButton = Text::create(this, Position(-1.0f, -1.0f, LengthType::PIXEL, getPosition().getRelativeTo(), getPosition().getReferencePoint()), buttonsTextSkin, leftButtonString);
        setupLeftButtonListeners();
        float leftButtonXPos = 0.0f;
        float leftButtonYPos = (float)(getHeight() - leftButton->getHeight()) / 2.0f;
        leftButton->updatePosition(Position(leftButtonXPos, leftButtonYPos, LengthType::PIXEL));

        rightButton = Text::create(this, Position(-1.0f, -1.0f, LengthType::PIXEL), buttonsTextSkin, rightButtonString);
        setupRightButtonListeners();
        float rightButtonXPos = (float)getWidth() - (float)rightButton->getWidth();
        float rightButtonYPos = (float)(getHeight() - rightButton->getHeight()) / 2.0f;
        rightButton->updatePosition(Position(rightButtonXPos, rightButtonYPos, LengthType::PIXEL));

        //values
        valuesText.resize(values.size(), std::shared_ptr<Text>(nullptr));
        for (std::size_t i = 0; i < values.size(); ++i) {
            if (translatableValues) {
                valuesText[i] = Text::create(this, Position(-1.0f, -1.0f, LengthType::PIXEL), valuesTextSkin, i18n(values[i]));
            } else {
                valuesText[i] = Text::create(this, Position(-1.0f, -1.0f, LengthType::PIXEL), valuesTextSkin, values[i]);
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
        setupLeftButtonListeners();
    }

    void Sequence::setRightButtonEventListener(std::shared_ptr<EventListener> rightButtonEventListener) {
        this->rightButtonEventListener = std::move(rightButtonEventListener);
        setupRightButtonListeners();
    }

    void Sequence::setupLeftButtonListeners() {
        if (leftButton) {
            leftButton->clearEventListeners();
            leftButton->addEventListener(std::make_unique<ButtonSequenceEventListener>(this, true));
            if (this->leftButtonEventListener) {
                leftButton->addEventListener(this->leftButtonEventListener);
            }
        }
    }

    void Sequence::setupRightButtonListeners() {
        if (rightButton) {
            rightButton->clearEventListeners();
            rightButton->addEventListener(std::make_unique<ButtonSequenceEventListener>(this, false));
            if (this->rightButtonEventListener) {
                rightButton->addEventListener(this->rightButtonEventListener);
            }
        }
    }

    void Sequence::prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) {
        //update the text position because the text size is updated when the UI language is changed
        float posX = ((float)getWidth() - (float)valuesText[selectedIndex]->getWidth()) / 2.0f;
        float posY = ((float)getHeight() - (float)valuesText[selectedIndex]->getHeight()) / 2.0f;
        valuesText[selectedIndex]->updatePosition(Position(posX, posY, LengthType::PIXEL));
    }

    Sequence::ButtonSequenceEventListener::ButtonSequenceEventListener(Sequence* sequence, bool isLeftButton) :
            sequence(sequence),
            isLeftButton(isLeftButton) {

    }

    bool Sequence::ButtonSequenceEventListener::onMouseLeftClickRelease(Widget*) {
        bool propagateEvent = true;
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
                propagateEvent = false;
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
                propagateEvent = false;
            }
        }

        if (oldSelectedIndex != sequence->selectedIndex) {
            for (auto& eventListener : sequence->getEventListeners()) {
                eventListener->onValueChange(sequence);
            }
        }
        return propagateEvent;
    }

}
