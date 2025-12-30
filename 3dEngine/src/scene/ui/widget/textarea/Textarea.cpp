#include "scene/ui/widget/textarea/Textarea.h"
#include "scene/ui/widget/TextFieldConst.h"
#include "scene/InputDeviceKey.h"

namespace urchin {

    Textarea::Textarea(Position position, Size size, std::string skinName) :
            Widget(std::move(position), size),
            skinName(std::move(skinName)),
            maxCharacter(-1),
            scrollbarWidthInPixel(0.0f),
            cursorIndex(0),
            cursorBlink(0.0f),
            selectModeOn(false),
            selectionStartIndex(0),
            ctrlKeyPressed(false),
            state(INACTIVE) {

    }

    std::shared_ptr<Textarea> Textarea::create(Widget* parent, Position position, Size size, std::string skinName) {
        return Widget::create<Textarea>(new Textarea(std::move(position), size, std::move(skinName)), parent);
    }

    WidgetType Textarea::getWidgetType() const {
        return WidgetType::TEXTAREA;
    }

    std::string Textarea::getText() const {
        return text->getBaseText();
    }

    const Text& Textarea::getTextWidget() const {
        return *text;
    }

    void Textarea::updateText(const std::string& text) {
        StringUtil::readCodepoints(text, this->originalText);
        refreshText(true);

        this->cursorIndex = originalText.size();
        refreshCursorPosition(this->cursorIndex);
        resetSelection();
    }

    void Textarea::setAllowedCharacters(const std::string& allowedCharacters) {
        StringUtil::readCodepoints(allowedCharacters, this->allowedCharacters);
    }

    void Textarea::setMaxCharacter(unsigned int maxCharacter) {
        this->maxCharacter = (int)maxCharacter;
    }

    unsigned int Textarea::getMaxCharacter() const {
        return (unsigned int)maxCharacter;
    }

    unsigned int Textarea::getCharacterCount() const {
        return (unsigned int)originalText.size();
    }

    void Textarea::createOrUpdateWidget() {
        //delete children
        detachChild(textContainer.get());

        //skin information
        auto textareaChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textarea", UdaAttribute("skin", skinName));

        auto skinChunkDefault = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "default"), textareaChunk);
        texTextareaDefault = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunkDefault, &getOutline());
        changeTexture(texTextareaDefault);

        auto skinChunkFocus = UISkinService::instance().getSkinReader().getFirstChunk(false, "skin", UdaAttribute("type", "focus"), textareaChunk);
        if (skinChunkFocus) {
            texTextareaFocus = UISkinService::instance().createWidgetTexture((unsigned int) getWidth(), (unsigned int) getHeight(), skinChunkFocus);
        } else {
            texTextareaFocus = texTextareaDefault;
        }

        auto scrollbarSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "scrollbarSkin", UdaAttribute(), textareaChunk);
        std::string scrollbarSkinName = scrollbarSkinChunk->getStringValue();
        auto scrollbarChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "scrollbar", UdaAttribute("skin", scrollbarSkinName));
        Length scrollbarWidth = UISkinService::instance().loadLength(scrollbarChunk, "width");
        scrollbarWidthInPixel = widthLengthToPixel(scrollbarWidth.value, scrollbarWidth.type, [] {return 0.0f;}) + TextFieldConst::TEXT_AND_SCROLLBAR_SHIFT;
        textContainer = Container::createScrollable(this, Position(0.0f, 0.0f, SCREEN_PERCENT), Size(100.0f, 100.0f, PARENT_PERCENT), scrollbarSkinName);

        auto textSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textSkin", UdaAttribute(), textareaChunk);
        text = Text::create(textContainer.get(), Position(0.0f, 0.0f, PIXEL), textSkinChunk->getStringValue(), "");
        float maxWidthText = getWidth() - (float)(getOutline().leftWidth + getOutline().rightWidth) - scrollbarWidthInPixel;
        text->setMaxWidth(maxWidthText, PIXEL);

        Vector3<float> fontColor = text->getFont().getFontColor();
        std::vector<unsigned char> cursorColor = {static_cast<unsigned char>(fontColor.X * 255), static_cast<unsigned char>(fontColor.Y * 255), static_cast<unsigned char>(fontColor.Z * 255), 255};
        texCursorAlbedo = Texture::build("cursor albedo", 1, 1, TextureFormat::RGBA_8_UINT_NORM, cursorColor.data(), false, TextureDataType::INT_8);
        Size cursorSize(TextFieldConst::CURSOR_WIDTH_PIXEL, (float)text->getFont().getHeight() + ((float)TextFieldConst::CURSOR_HEIGHT_MARGIN_PIXEL * 2.0f), PIXEL);
        cursor = StaticBitmap::create(textContainer.get(), Position(0.0f, 0.0f, PIXEL), cursorSize, texCursorAlbedo);
        cursor->setIsVisible(false);

        Vector4<float> selectionColor = UISkinService::instance().getSkinReader().getFirstChunk(true, "selectionColor", UdaAttribute(), textareaChunk)->getVector4Value();
        bool hasTransparency = !MathFunction::isOne(selectionColor.W);
        selectionTexture = Texture::build("textSelection", 1, 1, TextureFormat::RGBA_32_FLOAT, &selectionColor, hasTransparency, TextureDataType::FLOAT_32);

        refreshText(false);
        refreshCursorPosition(cursorIndex);
    }

    bool Textarea::requireRenderer() const {
        return true;
    }

    bool Textarea::onKeyPressEvent(InputDeviceKey key) {
        if (key == InputDeviceKey::CTRL) {
            ctrlKeyPressed = true;
        } else if (key == InputDeviceKey::MOUSE_LEFT) {
            if (widgetRectangle().collideWithPoint(Point2(getMouseX(), getMouseY()))) {
                state = ACTIVE;
                changeTexture(texTextareaFocus);

                Rectangle2D textZone(
                        Point2((int)getGlobalPositionX(), (int)getGlobalPositionY()),
                        Point2((int)getGlobalPositionX() + (int)getWidth() - (int)scrollbarWidthInPixel, (int)getGlobalPositionY() + (int)getHeight()));
                if (textZone.collideWithPoint(Point2(getMouseX(), getMouseY()))) {
                    int localMouseX = getMouseX() - MathFunction::roundToInt(text->getGlobalPositionX());
                    int localMouseY = getMouseY() - MathFunction::roundToInt(text->getGlobalPositionY());
                    cursorIndex = computeCursorIndex(localMouseX, localMouseY);
                    refreshCursorPosition(cursorIndex);
                    resetSelection();
                    selectModeOn = true;
                }
            } else {
                state = INACTIVE;
                changeTexture(texTextareaDefault);
                cursor->setIsVisible(false);
                resetSelection();
            }
        } else if (state == ACTIVE) {
            if (key == InputDeviceKey::A) {
                if (ctrlKeyPressed) {
                    selectionStartIndex = 0;
                    cursorIndex = originalText.size();
                    refreshCursorPosition(cursorIndex);
                    displaySelection();
                }
            } else if (key == InputDeviceKey::C) {
                if (ctrlKeyPressed && hasTextSelected()) {
                    std::span<char32_t> selectedText = std::span(originalText).subspan(
                        std::min(selectionStartIndex, cursorIndex),
                        std::max(selectionStartIndex, cursorIndex) - std::min(selectionStartIndex, cursorIndex));
                    getClipboard().setText(StringUtil::readUtf8String(selectedText));
                }
            } else if (key == InputDeviceKey::X) {
                if (ctrlKeyPressed && hasTextSelected()) {
                    std::span<char32_t> selectedText = std::span(originalText).subspan(
                            std::min(selectionStartIndex, cursorIndex),
                            std::max(selectionStartIndex, cursorIndex) - std::min(selectionStartIndex, cursorIndex));
                    getClipboard().setText(StringUtil::readUtf8String(selectedText));
                    deleteSelectedText();
                }
            } else if (key == InputDeviceKey::V) {
                if (ctrlKeyPressed && !getClipboard().getText().empty()) {
                    deleteSelectedText();
                    std::string textToPaste = getClipboard().getText();
                    const char* textIt = &textToPaste[0];
                    const char* textEndIt = textIt + textToPaste.size();
                    while (textIt != textEndIt) {
                        onCharEvent(StringUtil::readNextCodepoint(textIt, textEndIt));
                    }
                }
            } else if (key == InputDeviceKey::LEFT_ARROW) {
                if (hasTextSelected()) {
                    cursorIndex = std::min(cursorIndex, selectionStartIndex);
                    refreshCursorPosition(cursorIndex);
                } else if (cursorIndex > 0) {
                    cursorIndex--;
                    refreshCursorPosition(cursorIndex);
                }
                resetSelection();
            } else if (key == InputDeviceKey::RIGHT_ARROW) {
                if (hasTextSelected()) {
                    cursorIndex = std::max(cursorIndex, selectionStartIndex);
                    refreshCursorPosition(cursorIndex);
                } else if (cursorIndex < originalText.size()) {
                    cursorIndex++;
                    refreshCursorPosition(cursorIndex);
                }
                resetSelection();
            } else if (key == InputDeviceKey::UP_ARROW) {
                cursorIndex = computeCursorIndex(cursorPosition.X, cursorPosition.Y - (int)text->getFont().getSpaceBetweenLines());
                refreshCursorPosition(cursorIndex);
                resetSelection();
            } else if (key == InputDeviceKey::DOWN_ARROW) {
                cursorIndex = computeCursorIndex(cursorPosition.X, cursorPosition.Y + (int)text->getFont().getSpaceBetweenLines());
                refreshCursorPosition(cursorIndex);
                resetSelection();
            } else if (key == InputDeviceKey::BACKSPACE) {
                if (hasTextSelected()) {
                    deleteSelectedText();
                } else if (cursorIndex > 0) {
                    originalText.erase(originalText.begin() + ((long)cursorIndex - 1));
                    refreshText(true);

                    cursorIndex--;
                    refreshCursorPosition(cursorIndex);
                    resetSelection();
                }
            } else if (key == InputDeviceKey::DELETE_KEY) {
                if (hasTextSelected()) {
                    deleteSelectedText();
                } else if (cursorIndex < originalText.size()) {
                    originalText.erase(originalText.begin() + (long)cursorIndex);
                    refreshText(true);

                    refreshCursorPosition(cursorIndex);
                    resetSelection();
                }
            } else if (key == InputDeviceKey::ENTER) {
                if (hasTextSelected()) {
                    deleteSelectedText();
                }

                originalText.insert(originalText.begin() + (long)cursorIndex, U'\n');
                refreshText(true);

                cursorIndex++;
                refreshCursorPosition(cursorIndex);
                resetSelection();
            }
        }
        return true;
    }

    bool Textarea::onKeyReleaseEvent(InputDeviceKey key) {
        if (key == InputDeviceKey::CTRL) {
            ctrlKeyPressed = false;
        } else if (key == InputDeviceKey::MOUSE_LEFT) {
            if (selectModeOn) {
                selectModeOn = false;
                return false;
            }
        }
        return true;
    }

    bool Textarea::onCharEvent(char32_t unicodeCharacter) {
        if (state == ACTIVE) {
            if (isCharacterAllowed(unicodeCharacter)) {
                if (hasTextSelected()) {
                    deleteSelectedText();
                }

                if (!isMaxCharacterReach()) {
                    originalText.insert(originalText.begin() + (long)cursorIndex, unicodeCharacter);
                    refreshText(true);

                    cursorIndex++;
                    refreshCursorPosition(cursorIndex);
                    resetSelection();
                }
            }
            return false;
        }
        return true;
    }

    bool Textarea::onMouseMoveEvent(int mouseX, int mouseY) {
        if (selectModeOn) {
            int localMouseX = mouseX - MathFunction::roundToInt(text->getGlobalPositionX());
            int localMouseY = mouseY - MathFunction::roundToInt(text->getGlobalPositionY());

            cursorIndex = computeCursorIndex(localMouseX, localMouseY);
            refreshCursorPosition(cursorIndex);
            displaySelection();
            return false;
        }
        return true;
    }

    void Textarea::onResetStateEvent() {
        state = INACTIVE;
        changeTexture(texTextareaDefault);
    }

    bool Textarea::isCharacterAllowed(char32_t unicodeCharacter) const {
        if (!UnicodeUtil::isCharacterDisplayable(unicodeCharacter) && unicodeCharacter != '\n') {
            return false;
        }
        return allowedCharacters.empty() || std::ranges::find(allowedCharacters, unicodeCharacter) != allowedCharacters.end();
    }

    bool Textarea::isMaxCharacterReach() const {
        if (maxCharacter == -1) {
            return false;
        }
        return (int)getCharacterCount() >= maxCharacter;
    }

    void Textarea::refreshText(bool textUpdated) {
        text->updateText(StringUtil::readUtf8String(originalText));
        textContainer->notifyChildrenUpdated(); //ugly

        if (textUpdated) {
            for (auto& eventListener : getEventListeners()) {
                eventListener->onValueChange(this);
            }
        }
    }

    Point2<int> Textarea::computeCursorPosition(std::size_t cursorIdx, WordCutIndexPositioning wordCutIndexPositioning) const {
        std::size_t textCursorIndex = text->baseTextIndexToCutTextIndex(cursorIdx, wordCutIndexPositioning);
        std::size_t currentIndex = 0;

        Point2<int> computedCursorPosition(0.0f, 0.0f);
        computedCursorPosition.Y = 0.0f;
        for (const TextLine& textLine : text->getCutTextLines()) {
            computedCursorPosition.X = 0.0f;
            for (std::size_t columnIndex = 0; columnIndex <= textLine.text.length(); ++columnIndex) {
                if (currentIndex == textCursorIndex) {
                    return computedCursorPosition;
                } else {
                    char32_t textLetter = textLine.text[columnIndex];
                    computedCursorPosition.X += text->getFont().getGlyph(textLetter).letterWidth;

                    currentIndex++;
                }
            }
            computedCursorPosition.Y += (int)text->getFont().getSpaceBetweenLines();
        }
        throw std::runtime_error("Cursor position not found at index " + std::to_string(textCursorIndex) + " for text: " + StringUtil::readUtf8String(originalText));
    }

    void Textarea::refreshCursorPosition(std::size_t cursorIdx) {
        cursorPosition = computeCursorPosition(cursorIdx);
        adjustScrollToCursor();
        cursorBlink = 0.0f;

        cursor->updatePosition(Position((float)cursorPosition.X, (float)cursorPosition.Y - (float)TextFieldConst::CURSOR_HEIGHT_MARGIN_PIXEL, PIXEL));
        textContainer->notifyChildrenUpdated(); //ugly
    }

    void Textarea::adjustScrollToCursor() const {
        float cursorPosY = (float)cursorPosition.Y + (float)textContainer->getScrollShiftY();

        float deltaScrollShiftPixel = 0.0f;
        if (cursorPosY < textContainer->getPositionY()) {
            deltaScrollShiftPixel = textContainer->getPositionY() - cursorPosY;
        } else if (cursorPosY + cursor->getHeight() > (textContainer->getPositionY() + textContainer->getHeight())) {
            deltaScrollShiftPixel = (textContainer->getPositionY() + textContainer->getHeight()) - (cursorPosY + cursor->getHeight());
        }

        int scrollShiftPixel = textContainer->getScrollShiftY() + MathFunction::roundToInt(deltaScrollShiftPixel);
        textContainer->updateScrollShiftY(scrollShiftPixel);
    }

    std::size_t Textarea::computeCursorIndex(int approximatePositionX, int approximatePositionY) const {
        std::size_t textCursorIndex = 0;

        float currentHeight = 0.0;
        for (std::size_t lineIndex = 0; lineIndex < text->getCutTextLines().size(); ++lineIndex) {
            if (lineIndex != 0) {
                textCursorIndex++; //move cursor from previous line end to beginning of current line
            }

            currentHeight += (float)text->getFont().getSpaceBetweenLines();
            if ((float)approximatePositionY < currentHeight) {
                float currentWidth = 0.0f;
                for (std::size_t columnIndex = 0; columnIndex <= text->getCutTextLines()[lineIndex].text.length(); ++columnIndex) {
                    if (columnIndex != 0) {
                        textCursorIndex++;
                    }

                    char32_t textLetter = text->getCutTextLines()[lineIndex].text[columnIndex];
                    currentWidth += (float)text->getFont().getGlyph(textLetter).letterWidth / 2.0f;
                    if ((float)approximatePositionX < currentWidth) {
                        break;
                    }
                    currentWidth += (float)text->getFont().getGlyph(textLetter).letterWidth / 2.0f;
                }
                break;
            } else {
                textCursorIndex += text->getCutTextLines()[lineIndex].text.length(); //index at end of line
            }
        }

        return text->cutTextIndexToBaseTextIndex(textCursorIndex);
    }

    bool Textarea::hasTextSelected() const {
        return selectionStartIndex != cursorIndex;
    }

    void Textarea::resetSelection() {
        selectionStartIndex = cursorIndex;
        resetDisplaySelection();
    }

    void Textarea::resetDisplaySelection() {
        std::ranges::for_each(selectionImages, [&](const std::shared_ptr<StaticBitmap>& selectionImg){ textContainer->detachChild(selectionImg.get()); });
        selectionImages.clear();
    }

    void Textarea::displaySelection() {
        resetDisplaySelection();

        std::size_t displaySelectionStartIndex = std::min(selectionStartIndex, cursorIndex);
        std::size_t displaySelectionEndIndex = std::max(selectionStartIndex, cursorIndex);
        std::size_t currentSelectionIndex = displaySelectionStartIndex;
        std::size_t lineIndex = 0;

        while (currentSelectionIndex < displaySelectionEndIndex) {
            std::size_t endOfCurrentLineIndex = text->baseTextIndexToEndOfLineIndex(currentSelectionIndex);
            endOfCurrentLineIndex = std::min(endOfCurrentLineIndex, displaySelectionEndIndex);

            Point2<int> displaySelectionStartPos = computeCursorPosition(currentSelectionIndex, WordCutIndexPositioning::BEGIN_OF_NEXT_LINE) + Point2(0, -(int)TextFieldConst::CURSOR_HEIGHT_MARGIN_PIXEL);
            Point2<int> displaySelectionEndPos = computeCursorPosition(endOfCurrentLineIndex, WordCutIndexPositioning::END_OF_LINE) + Point2(0, (int)text->getFont().getHeight() + (int)TextFieldConst::CURSOR_HEIGHT_MARGIN_PIXEL * 2);

            Position selectionPosition((float) displaySelectionStartPos.X, (float) displaySelectionStartPos.Y, PIXEL);
            Size selectionSize((float) (displaySelectionEndPos.X - displaySelectionStartPos.X), (float) (displaySelectionEndPos.Y - displaySelectionStartPos.Y), PIXEL);
            std::shared_ptr<StaticBitmap> selectionImg = StaticBitmap::create(textContainer.get(), selectionPosition, selectionSize, selectionTexture);
            selectionImages.push_back(std::move(selectionImg));

            currentSelectionIndex = endOfCurrentLineIndex;
            if (text->getCutTextLines()[lineIndex].cutType != TextCutType::MIDDLE_WORD) {
                currentSelectionIndex++; //for line return
            }
            lineIndex++;
        }
    }

    void Textarea::deleteSelectedText() {
        if (selectionStartIndex != cursorIndex) {
            long startDeletionIndex = (long)std::min(selectionStartIndex, cursorIndex);
            long endDeletionIndex = (long)std::max(selectionStartIndex, cursorIndex);
            originalText.erase(originalText.begin() + startDeletionIndex, originalText.begin() + endDeletionIndex);
        }
        refreshText(true);

        cursorIndex = std::min(selectionStartIndex, cursorIndex);
        refreshCursorPosition(cursorIndex);
        resetSelection();
    }

    void Textarea::prepareWidgetRendering(float dt) {
        //cursor
        cursorBlink += dt * TextFieldConst::CURSOR_BLINK_SPEED;
        if (state == ACTIVE) {
            if (((int)cursorBlink % 2) == 0) {
                cursor->setIsVisible(true);
            } else {
                cursor->setIsVisible(false);
            }
        }
    }

}
