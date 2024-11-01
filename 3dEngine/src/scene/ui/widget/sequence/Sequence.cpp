#include <stdexcept>
#include <utility>

#include <scene/ui/widget/sequence/Sequence.h>
#include <scene/ui/UISkinService.h>

namespace urchin {

    Sequence::Sequence(Position position, Size size, std::string skinName, const std::vector<ParameterizedText>& values) :
            Widget(position, size),
            skinName(std::move(skinName)),
            values(values),
            loopOnValuesEnabled(true),
            selectedIndex(0),
            leftButton(nullptr),
            rightButton(nullptr) {
        if (values.empty()) {
            throw std::runtime_error("At least one value/key must be provided to sequence.");
        }
    }

    std::shared_ptr<Sequence> Sequence::create(Widget* parent, Position position, Size size, std::string skinName, const std::vector<std::string>& texts) {
        std::vector<ParameterizedText> parameterizedTexts;
        parameterizedTexts.reserve(texts.size());
        for (const std::string& text : texts) {
            parameterizedTexts.emplace_back(text);
        }
        return Widget::create<Sequence>(new Sequence(position, size, std::move(skinName), parameterizedTexts), parent);
    }

    std::shared_ptr<Sequence> Sequence::create(Widget* parent, Position position, Size size, std::string skinName, const std::vector<ParameterizedText>& texts) {
        return Widget::create<Sequence>(new Sequence(position, size, std::move(skinName), texts), parent);
    }

    void Sequence::createOrUpdateWidget() {
        //detach children
        for (const auto& valueText : valuesText) {
            detachChild(valueText.get());
        }
        detachChild(rightButton.get());
        detachChild(leftButton.get());

        //skin information
        auto sequenceChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "sequence", UdaAttribute("skin", skinName));

        auto buttonsTextSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "buttonsTextSkin", UdaAttribute(), sequenceChunk);
        std::string buttonsTextSkin = buttonsTextSkinChunk->getStringValue();

        auto valuesTextSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "valuesTextSkin", UdaAttribute(), sequenceChunk);
        std::string valuesTextSkin = valuesTextSkinChunk->getStringValue();

        auto leftButtonTextChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "leftButtonText", UdaAttribute(), sequenceChunk);
        std::string leftButtonString = leftButtonTextChunk->getStringValue();

        auto rightButtonTextChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "rightButtonText", UdaAttribute(), sequenceChunk);
        std::string rightButtonString = rightButtonTextChunk->getStringValue();

        //buttons
        leftButton = Text::create(this, Position(-1.0f, -1.0f, PIXEL, getPosition().getRelativeTo(), getPosition().getReferencePoint()), buttonsTextSkin, leftButtonString);
        setupLeftButtonListeners();
        float leftButtonXPos = 0.0f;
        float leftButtonYPos = (getHeight() - leftButton->getHeight()) / 2.0f;
        leftButton->updatePosition(Position(leftButtonXPos, leftButtonYPos, PIXEL));

        rightButton = Text::create(this, Position(-1.0f, -1.0f, PIXEL), buttonsTextSkin, rightButtonString);
        setupRightButtonListeners();
        float rightButtonXPos = getWidth() - rightButton->getWidth();
        float rightButtonYPos = (getHeight() - rightButton->getHeight()) / 2.0f;
        rightButton->updatePosition(Position(rightButtonXPos, rightButtonYPos, PIXEL));

        //values
        valuesText.resize(values.size(), std::shared_ptr<Text>(nullptr));
        for (std::size_t i = 0; i < values.size(); ++i) {
            valuesText[i] = Text::create(this, Position(-1.0f, -1.0f, PIXEL), valuesTextSkin, values[i]);
            valuesText[i]->setIsVisible(false);
        }
        valuesText[selectedIndex]->setIsVisible(true);
    }

    WidgetType Sequence::getWidgetType() const {
        return WidgetType::SEQUENCE;
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

    void Sequence::prepareWidgetRendering(float) {
        //update the text position because the text size is updated when the UI language is changed
        float posX = (getWidth() - valuesText[selectedIndex]->getWidth()) / 2.0f;
        float posY = (getHeight() - valuesText[selectedIndex]->getHeight()) / 2.0f;
        valuesText[selectedIndex]->updatePosition(Position(posX, posY, PIXEL));
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
