#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <utility>

#include "scene/ui/widget/sequence/Sequence.h"
#include "scene/ui/UISkinService.h"

namespace urchin {

    Sequence::Sequence(Position position, Size size, std::string skinName, const std::vector<ParameterizedText>& values) :
            Widget(std::move(position), size),
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
        return Widget::create<Sequence>(new Sequence(std::move(position), size, std::move(skinName), parameterizedTexts), parent);
    }

    std::shared_ptr<Sequence> Sequence::create(Widget* parent, Position position, Size size, std::string skinName, const std::vector<ParameterizedText>& texts) {
        return Widget::create<Sequence>(new Sequence(std::move(position), size, std::move(skinName), texts), parent);
    }

    void Sequence::createOrUpdateWidget() {
        //detach children
        for (const auto& valueText : valuesText) {
            detachChild(valueText.get());
        }
        detachChild(rightButton.get());
        detachChild(leftButton.get());
        detachChild(progressLineImage.get());
        detachChild(progressCursorImage.get());

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
        leftButton = Container::create(this, Position(0.0f, 0.0f, PIXEL), Size(0.0f, getHeight(), PIXEL));
        auto leftButtonText = Text::create(leftButton.get(), Position(0.0f, 0.0f, PIXEL), buttonsTextSkin, leftButtonString);
        leftButtonText->updatePosition(Position(0.0f, (getHeight() - leftButtonText->getHeight()) / 2.0f, PIXEL));
        leftButton->updateSize(Size(leftButtonText->getWidth(), getHeight(), PIXEL));
        setupLeftButtonListeners();

        rightButton = Container::create(this, Position(0.0f, 0.0f, PIXEL), Size(0.0f, getHeight(), PIXEL));
        auto rightButtonText = Text::create(rightButton.get(), Position(0.0f, 0.0f, PIXEL), buttonsTextSkin, rightButtonString);
        rightButtonText->updatePosition(Position(0.0f, (getHeight() - rightButtonText->getHeight()) / 2.0f, PIXEL));
        rightButton->updateSize(Size(rightButtonText->getWidth(), getHeight(), PIXEL));
        rightButton->updatePosition(Position(getWidth() - rightButtonText->getWidth(), 0.0f, PIXEL));
        setupRightButtonListeners();

        //values
        valuesText.resize(values.size(), std::shared_ptr<Text>(nullptr));
        for (std::size_t i = 0; i < values.size(); ++i) {
            valuesText[i] = Text::create(this, Position(-1.0f, -1.0f, PIXEL), valuesTextSkin, values[i]);
            valuesText[i]->setIsVisible(false);
        }
        valuesText[selectedIndex]->setIsVisible(true);

        //progress bar
        auto progressBarChunk = UISkinService::instance().getSkinReader().getFirstChunk(false, "progressBar", UdaAttribute(), sequenceChunk);
        if (progressBarChunk) {
            auto imageLineChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "imageLine", UdaAttribute(), progressBarChunk);
            auto imageCursorChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "imageCursor", UdaAttribute(), progressBarChunk);

            float progressBarPosXStart = leftButton->getWidth() + getWidth() * PROGRESS_BAR_SHIFT_LEFT_PERC;
            float progressBarPosYStart = -getHeight() * PROGRESS_BAR_SHIFT_TOP_PERC;
            float progressBarWidth = getWidth() - 2.0f * progressBarPosXStart;
            float progressBarHeight = std::max(1.0f, std::round(PROGRESS_BAR_HEIGHT_RATIO * getHeight()));
            progressLineImage = StaticBitmap::create(this, Position(progressBarPosXStart, progressBarPosYStart, PIXEL, PARENT_LEFT_BOTTOM),
                Size(progressBarWidth, progressBarHeight, PIXEL), imageLineChunk->getStringValue());
            progressCursorImage = StaticBitmap::create(this, Position(progressBarPosXStart, progressBarPosYStart, PIXEL, PARENT_LEFT_BOTTOM),
                Size(progressBarWidth / (float)values.size(), progressBarHeight, PIXEL), imageCursorChunk->getStringValue());
            moveProgressCursor();
        }
    }

    void Sequence::moveProgressCursor() const {
        if (progressCursorImage) {
            float cursorXPos = progressLineImage->getPositionX() + (float)selectedIndex * (progressLineImage->getWidth() / (float)values.size());
            progressCursorImage->updatePosition(Position(cursorXPos, progressLineImage->getPositionY(), PIXEL, PARENT_LEFT_BOTTOM));
        }
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
        moveProgressCursor();
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

    bool Sequence::onKeyPressEvent(InputKey key) {
        if (isGameControllerFocused()) {
            if (InputKeyNavigation::isLeft(key)) {
                return triggerButton(*leftButton);
            } else if (InputKeyNavigation::isRight(key)) {
                return triggerButton(*rightButton);
            }
        }
        return true;
    }

    bool Sequence::triggerButton(Widget& button) const {
        bool propagateEvent = true;
        for (const auto& eventListener : button.getEventListeners()) {
            propagateEvent &= eventListener->onMouseLeftClickRelease(&button);
        }
        return propagateEvent;
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
            sequence->moveProgressCursor();
            for (auto& eventListener : sequence->getEventListeners()) {
                eventListener->onValueChange(sequence);
            }
        }
        return propagateEvent;
    }

}
