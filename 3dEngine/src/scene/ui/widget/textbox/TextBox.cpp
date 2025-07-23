#include <utility>
#include <UrchinCommon.h>

#include "scene/ui/widget/textbox/TextBox.h"
#include "scene/ui/widget/TextFieldConst.h"
#include "scene/ui/displayer/WidgetInstanceDisplayer.h"
#include "scene/InputDeviceKey.h"

namespace urchin {

    TextBox::TextBox(Position position, Size size, std::string skinName) :
            Widget(std::move(position), size),
            skinName(std::move(skinName)),
            maxCharacter(-1),
            text(nullptr),
            maxWidthText(0),
            startTextIndex(0),
            cursorIndex(0),
            cursorBlink(0.0f),
            selectModeOn(false),
            selectionStartIndex(0),
            ctrlKeyPressed(false),
            state(INACTIVE) {

    }

    std::shared_ptr<TextBox> TextBox::create(Widget* parent, Position position, Size size, std::string skinName) {
        return Widget::create<TextBox>(new TextBox(std::move(position), size, std::move(skinName)), parent);
    }

    void TextBox::createOrUpdateWidget() {
        //detach children
        detachChild(cursor.get());
        detachChild(text.get());

        //skin information
        auto textBoxChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textBox", UdaAttribute("skin", skinName));

        auto skinChunkDefault = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "default"), textBoxChunk);
        texTextBoxDefault = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunkDefault, &getOutline());
        changeTexture(texTextBoxDefault);

        auto skinChunkFocus = UISkinService::instance().getSkinReader().getFirstChunk(false, "skin", UdaAttribute("type", "focus"), textBoxChunk);
        if (skinChunkFocus) {
            texTextBoxFocus = UISkinService::instance().createWidgetTexture((unsigned int) getWidth(), (unsigned int) getHeight(), skinChunkFocus);
        } else {
            texTextBoxFocus = texTextBoxDefault;
        }

        auto textSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textSkin", UdaAttribute(), textBoxChunk);
        text = Text::create(this, Position(0.0f, 0.0f, PIXEL, PARENT_LEFT_CENTERY, LEFT_CENTERY), textSkinChunk->getStringValue(), "");
        maxWidthText = (unsigned int)((int)getWidth() - (getOutline().leftWidth + getOutline().rightWidth + (int)TextFieldConst::LETTER_AND_CURSOR_SHIFT + (int)TextFieldConst::CURSOR_WIDTH_PIXEL));

        Vector3<float> fontColor = text->getFont().getFontColor();
        std::vector<unsigned char> cursorColor = {static_cast<unsigned char>(fontColor.X * 255), static_cast<unsigned char>(fontColor.Y * 255), static_cast<unsigned char>(fontColor.Z * 255), 255};
        texCursorAlbedo = Texture::build("cursor albedo", 1, 1, TextureFormat::RGBA_8_INT, cursorColor.data());
        Size cursorSize(TextFieldConst::CURSOR_WIDTH_PIXEL, (float)text->getFont().getHeight() + ((float)TextFieldConst::CURSOR_HEIGHT_MARGIN_PIXEL * 2.0f), PIXEL);
        cursor = StaticBitmap::create(this, Position(0.0f, 0.0f, PIXEL), cursorSize, texCursorAlbedo);
        cursor->setIsVisible(false);

        Vector4<float> selectionColor = UISkinService::instance().getSkinReader().getFirstChunk(true, "selectionColor", UdaAttribute(), textBoxChunk)->getVector4Value();
        std::shared_ptr<Texture> selectionTexture = Texture::build("textSelection", 1, 1, TextureFormat::RGBA_32_FLOAT, &selectionColor);
        selectionImage = StaticBitmap::create(this, Position(0.0f, 0.0f, PIXEL), Size(0.0f, 0.0f, PIXEL), selectionTexture);
        selectionImage->setIsVisible(false);

        refreshText(false);
        refreshCursorPosition(cursorIndex);
    }

    bool TextBox::requireRenderer() const {
        return true;
    }

    WidgetType TextBox::getWidgetType() const {
        return WidgetType::TEXTBOX;
    }

    std::string TextBox::getText() {
        return std::string(stringConvert.to_bytes(originalText));
    }

    const Text& TextBox::getTextWidget() const {
        return *text;
    }

    void TextBox::updateText(const std::string& text) {
        originalText = stringConvert.from_bytes(std::string(text).c_str());
        cursorIndex = originalText.length();
        refreshText(true);
    }

    void TextBox::setAllowedCharacters(const std::string& allowedCharacters) {
        this->allowedCharacters = stringConvert.from_bytes(allowedCharacters.c_str());
    }

    void TextBox::setMaxCharacter(unsigned int maxCharacter) {
        this->maxCharacter = (int)maxCharacter;
    }

    unsigned int TextBox::getMaxCharacter() const {
        return (unsigned int)maxCharacter;
    }

    unsigned int TextBox::getCharacterCount() const {
        return (unsigned int)originalText.length();
    }

    bool TextBox::onKeyPressEvent(InputDeviceKey key) {
        if (key == InputDeviceKey::CTRL) {
            ctrlKeyPressed = true;
        } else if (key == InputDeviceKey::MOUSE_LEFT) {
            if (widgetRectangle().collideWithPoint(Point2(getMouseX(), getMouseY()))) {
                state = ACTIVE;
                changeTexture(texTextBoxFocus);

                int localMouseX = getMouseX() - MathFunction::roundToInt(text->getGlobalPositionX());
                int localMouseY = getMouseY() - MathFunction::roundToInt(text->getGlobalPositionY());
                cursorIndex = computeCursorIndex(localMouseX, localMouseY);
                refreshCursorPosition(cursorIndex);
                resetSelection();
                selectModeOn = true;
            } else {
                state = INACTIVE;
                changeTexture(texTextBoxDefault);
                cursor->setIsVisible(false);
                resetSelection();
            }
        } else if (state == ACTIVE) {
            if (key == InputDeviceKey::A) {
                if (ctrlKeyPressed) {
                    selectionStartIndex = 0;
                    cursorIndex = originalText.length();
                    refreshCursorPosition(cursorIndex);
                    refreshText(false);
                    displaySelection();
                }
            } else if (key == InputDeviceKey::C) {
                if (ctrlKeyPressed && hasTextSelected()) {
                    U32StringA selectedText = originalText.substr(std::min(selectionStartIndex, cursorIndex), std::max(selectionStartIndex, cursorIndex) - std::min(selectionStartIndex, cursorIndex));
                    getClipboard().setText(std::string(stringConvert.to_bytes(selectedText)));
                }
            } else if (key == InputDeviceKey::X) {
                if (ctrlKeyPressed && hasTextSelected()) {
                    U32StringA selectedText = originalText.substr(std::min(selectionStartIndex, cursorIndex), std::max(selectionStartIndex, cursorIndex) - std::min(selectionStartIndex, cursorIndex));
                    getClipboard().setText(std::string(stringConvert.to_bytes(selectedText)));
                    deleteSelectedText();
                }
            } else if (key == InputDeviceKey::V) {
                if (ctrlKeyPressed && !getClipboard().getText().empty()) {
                    deleteSelectedText();
                    U32StringA clipboardString = stringConvert.from_bytes(getClipboard().getText().c_str());
                    for (char32_t charLetter : clipboardString) {
                        onCharEvent(charLetter);
                    }
                }
            } else if (key == InputDeviceKey::LEFT_ARROW) {
                if (hasTextSelected()) {
                    cursorIndex = std::min(cursorIndex, selectionStartIndex);
                    refreshText(false);
                } else if (cursorIndex > 0) {
                    cursorIndex--;
                    refreshText(false);
                }
                resetSelection();
            } else if (key == InputDeviceKey::RIGHT_ARROW) {
                if (hasTextSelected()) {
                    cursorIndex = std::max(cursorIndex, selectionStartIndex);
                    refreshText(false);
                } else if (cursorIndex < originalText.length()) {
                    cursorIndex++;
                    refreshText(false);
                }
                resetSelection();
            } else if (key == InputDeviceKey::BACKSPACE) {
                if (hasTextSelected()) {
                    deleteSelectedText();
                } else if (cursorIndex > 0) {
                    U32StringA tmpRight = originalText.substr(cursorIndex, originalText.length() - cursorIndex);
                    originalText = originalText.substr(0, cursorIndex - 1L);
                    originalText.append(tmpRight);

                    cursorIndex--;
                    refreshText(true);
                    resetSelection();
                }
            } else if (key == InputDeviceKey::DELETE_KEY) {
                if (hasTextSelected()) {
                    deleteSelectedText();
                } else if (cursorIndex < originalText.length()) {
                    U32StringA tmpRight = originalText.substr(cursorIndex + 1L, originalText.length() - cursorIndex);
                    originalText = originalText.substr(0, cursorIndex);
                    originalText.append(tmpRight);

                    refreshText(true);
                    resetSelection();
                }
            }
        }
        return true;
    }

    bool TextBox::onKeyReleaseEvent(InputDeviceKey key) {
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

    bool TextBox::onCharEvent(char32_t unicodeCharacter) {
        if (state == ACTIVE) {
            if (isCharacterAllowed(unicodeCharacter)) {
                if (hasTextSelected()) {
                    deleteSelectedText();
                }

                if (!isMaxCharacterReach()) {
                    U32StringA tmpRight = originalText.substr(cursorIndex, originalText.length() - cursorIndex);
                    originalText = originalText.substr(0, cursorIndex);
                    originalText.append(1, unicodeCharacter);
                    originalText.append(tmpRight);

                    cursorIndex++;
                    refreshText(true);

                    resetSelection();
                }
            }
            return false;
        }
        return true;
    }

    bool TextBox::onMouseMoveEvent(int mouseX, int mouseY) {
        if (selectModeOn) {
            int localMouseX = mouseX - MathFunction::roundToInt(text->getGlobalPositionX());
            int localMouseY = mouseY - MathFunction::roundToInt(text->getGlobalPositionY());

            cursorIndex = computeCursorIndex(localMouseX, localMouseY);
            refreshCursorPosition(cursorIndex);
            refreshText(false);
            displaySelection();
            return false;
        }
        return true;
    }

    void TextBox::onResetStateEvent() {
        state = INACTIVE;
        changeTexture(texTextBoxDefault);
    }

    bool TextBox::isCharacterAllowed(char32_t unicodeCharacter) const {
        if (!UnicodeUtil::isCharacterDisplayable(unicodeCharacter)) {
            return false;
        }
        return allowedCharacters.empty() || std::ranges::find(allowedCharacters, unicodeCharacter) != allowedCharacters.end();
    }

    bool TextBox::isMaxCharacterReach() const {
        return maxCharacter != -1 && (int)getCharacterCount() >= maxCharacter;
    }

    void TextBox::refreshText(bool textUpdated) {
        refreshCursorPosition(cursorIndex);

        if (cursorPosition.X > (int)maxWidthText) {
            std::size_t endTextIndex = std::min(cursorIndex + 1, originalText.length());

            unsigned int widthText = 0;
            for (startTextIndex = endTextIndex; true; --startTextIndex) {
                char32_t textLetter = originalText[startTextIndex];
                widthText += text->getFont().getGlyph(textLetter).width + text->getFont().getSpaceBetweenCharacters();
                if (widthText > maxWidthText || startTextIndex == 0) {
                    break;
                }
            }
        } else if (cursorIndex <= startTextIndex) {
            startTextIndex = (std::size_t) std::max((int) cursorIndex - 1, 0);
            startTextIndex = std::min(startTextIndex, originalText.length());
        }

        unsigned int widthText = 0;
        std::size_t endTextIndex;
        for (endTextIndex = startTextIndex; endTextIndex < originalText.length(); ++endTextIndex) {
            char32_t textLetter = originalText[endTextIndex];
            widthText += text->getFont().getGlyph(textLetter).width + text->getFont().getSpaceBetweenCharacters();
            if (widthText > maxWidthText) {
                break;
            }
        }

        U32StringA textToDisplay = originalText.substr(startTextIndex, endTextIndex - startTextIndex);
        text->updateText(std::string(stringConvert.to_bytes(textToDisplay)));

        refreshCursorPosition(cursorIndex);

        //event
        if (textUpdated) {
            for (auto& eventListener : getEventListeners()) {
                eventListener->onValueChange(this);
            }
        }
    }

    Point2<int> TextBox::computeCursorPosition(std::size_t cursorIdx) const {
        Point2<int> computedCursorPosition(0.0f, 0.0f);

        for (std::size_t i = startTextIndex; i < cursorIdx; ++i) {
            char32_t textLetter = originalText[i];
            computedCursorPosition.X += (int)(text->getFont().getGlyph(textLetter).width + text->getFont().getSpaceBetweenCharacters());
        }

        if (computedCursorPosition.X > 0) {
            computedCursorPosition.X -= (int)text->getFont().getSpaceBetweenCharacters(); //remove last space
            computedCursorPosition.X += TextFieldConst::LETTER_AND_CURSOR_SHIFT;
        }

        return computedCursorPosition;
    }

    void TextBox::refreshCursorPosition(std::size_t cursorIdx) {
        cursorPosition = computeCursorPosition(cursorIdx);
        cursorBlink = 0.0f;

        cursor->updatePosition(Position((float)cursorPosition.X, (float)cursorPosition.Y, PIXEL, PARENT_LEFT_CENTERY, LEFT_CENTERY));
    }

    std::size_t TextBox::computeCursorIndex(int approximatePositionX, int) const {
        const auto& font = text->getFont();
        float widthText = 0.0f;

        std::size_t computedCursorIndex = startTextIndex;
        for (; computedCursorIndex < originalText.length(); ++computedCursorIndex) {
            char32_t textLetter = originalText[computedCursorIndex];
            widthText += (float)font.getGlyph(textLetter).width / 2.0f;
            if (widthText > (float)approximatePositionX) {
                break;
            }

            widthText += (float)font.getGlyph(textLetter).width / 2.0f + (float)font.getSpaceBetweenCharacters();
        }
        return computedCursorIndex;
    }

    bool TextBox::hasTextSelected() const {
        return selectionStartIndex != cursorIndex;
    }

    void TextBox::resetSelection() {
        selectionStartIndex = cursorIndex;
        selectionImage->setIsVisible(false);
    }

    void TextBox::displaySelection() const {
        selectionImage->setIsVisible(true);

        std::size_t displaySelectionStartIndex = std::min(selectionStartIndex, cursorIndex);
        std::size_t displaySelectionEndIndex = std::max(selectionStartIndex, cursorIndex);

        Point2<int> displaySelectionStartPos = computeCursorPosition(displaySelectionStartIndex) + Point2(0, -(int)TextFieldConst::CURSOR_HEIGHT_MARGIN_PIXEL);
        Point2<int> displaySelectionEndPos = computeCursorPosition(displaySelectionEndIndex) + Point2(0, (int)text->getFont().getHeight() + (int)TextFieldConst::CURSOR_HEIGHT_MARGIN_PIXEL * 2);

        selectionImage->updatePosition(Position((float)displaySelectionStartPos.X, (float)displaySelectionStartPos.Y, PIXEL, PARENT_LEFT_CENTERY, LEFT_CENTERY));
        float sizeX = std::min((float)(displaySelectionEndPos.X - displaySelectionStartPos.X), (float)maxWidthText - (float)displaySelectionStartPos.X);
        selectionImage->updateSize(Size(sizeX, (float)(displaySelectionEndPos.Y - displaySelectionStartPos.Y), PIXEL));
    }

    void TextBox::deleteSelectedText() {
        U32StringA tmpRight = originalText.substr(std::max(selectionStartIndex, cursorIndex), originalText.length() - std::max(selectionStartIndex, cursorIndex));
        originalText = originalText.substr(0, std::min(selectionStartIndex, cursorIndex));
        originalText.append(tmpRight);

        cursorIndex = std::min(selectionStartIndex, cursorIndex);
        refreshCursorPosition(cursorIndex);
        refreshText(true);
        resetSelection();
    }

    void TextBox::prepareWidgetRendering(float dt) {
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
