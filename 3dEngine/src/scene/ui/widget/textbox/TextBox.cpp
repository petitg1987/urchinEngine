#include <utility>
#include <codecvt>
#include <UrchinCommon.h>

#include <scene/ui/widget/textbox/TextBox.h>
#include <scene/ui/widget/TextFieldConst.h>
#include <scene/InputDeviceKey.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    TextBox::TextBox(Position position, Size size, std::string skinName) :
            Widget(position, size),
            skinName(std::move(skinName)),
            maxCharacter(-1),
            text(nullptr),
            maxWidthText(0),
            startTextIndex(0),
            cursorIndex(0),
            cursorBlink(0.0f),
            selectModeOn(false),
            selectionStartIndex(0),
            state(INACTIVE) {

    }

    std::shared_ptr<TextBox> TextBox::create(Widget* parent, Position position, Size size, std::string skinName) {
        return Widget::create<TextBox>(new TextBox(position, size, std::move(skinName)), parent);
    }

    void TextBox::createOrUpdateWidget() {
        //detach children
        detachChild(cursor.get());
        detachChild(text.get());

        //skin information
        auto textBoxChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textBox", UdaAttribute("skin", skinName));

        auto skinChunkDefault = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "default"), textBoxChunk);
        texTextBoxDefault = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunkDefault, &widgetOutline);

        auto skinChunkFocus = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "focus"), textBoxChunk);
        texTextBoxFocus = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunkFocus);

        auto textSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textSkin", UdaAttribute(), textBoxChunk);
        text = Text::create(this, Position(0.0f, 0.0f, PIXEL, PARENT_LEFT_CENTERY, RefPoint::LEFT_CENTERY), textSkinChunk->getStringValue(), "");
        maxWidthText = (unsigned int)((int)getWidth() - (widgetOutline.leftWidth + widgetOutline.rightWidth));

        Vector3<float> fontColor = text->getFont().getFontColor();
        std::vector<unsigned char> cursorColor = {static_cast<unsigned char>(fontColor.X * 255), static_cast<unsigned char>(fontColor.Y * 255), static_cast<unsigned char>(fontColor.Z * 255), 255};
        texCursorAlbedo = Texture::build("cursor albedo", 1, 1, TextureFormat::RGBA_8_INT, cursorColor.data());
        Size cursorSize(TextFieldConst::CURSOR_WIDTH_PIXEL, (float)text->getFont().getHeight() + ((float)TextFieldConst::CURSOR_HEIGHT_MARGIN_PIXEL * 2.0f), PIXEL);
        cursor = StaticBitmap::create(this, Position(0.0f, 0.0f, PIXEL), cursorSize, texCursorAlbedo);
        cursor->setIsVisible(false);

        Vector4<float> selectionColor = UISkinService::instance().getSkinReader().getFirstChunk(true, "selectionColor", UdaAttribute(), textBoxChunk)->getVector4Value();
        std::shared_ptr<Texture> selectionTexture = Texture::build("textSelection", 1, 1, TextureFormat::RGBA_32_FLOAT, &selectionColor);
        selectionImg = StaticBitmap::create(this, Position(0.0f, 0.0f, PIXEL), Size(0.0f, 0.0f, PIXEL), selectionTexture);
        selectionImg->setIsVisible(false);

        refreshText(false);
        cursorPosition = computeCursorPosition(cursorIndex);

        //visual
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(getWidth(), 0.0f), Point2<float>(getWidth(), getHeight()),
                Point2<float>(0.0f, 0.0f), Point2<float>(getWidth(), getHeight()), Point2<float>(0.0f, getHeight())
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        textBoxRenderer = setupUiRenderer("text box", ShapeType::TRIANGLE, false)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(texTextBoxDefault, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy()))) //binding 3
                ->build();
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

    void TextBox::updateText(std::string_view text) {
        originalText = U32StringA(text.begin(), text.end());
        cursorIndex = (unsigned int)originalText.length();
        refreshText(true);
    }

    void TextBox::setAllowedCharacters(const std::string& allowedCharacters) {
        this->allowedCharacters = stringConvert.from_bytes(allowedCharacters.c_str());
    }

    void TextBox::setMaxCharacter(unsigned int maxCharacter) {
        this->maxCharacter = (int)maxCharacter;
    }

    bool TextBox::onKeyPressEvent(unsigned int key) {
        if (key == (int)InputDeviceKey::MOUSE_LEFT) {
            if (widgetRectangle().collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                state = ACTIVE;
                textBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texTextBoxFocus, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy())));

                int localMouseX = getMouseX() - MathFunction::roundToInt(text->getGlobalPositionX());
                int localMouseY = getMouseY() - MathFunction::roundToInt(text->getGlobalPositionY());
                cursorIndex = computeCursorIndex(localMouseX, localMouseY);
                cursorPosition = computeCursorPosition(cursorIndex);
                resetSelection();
                selectModeOn = true;
            } else {
                state = INACTIVE;
                textBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texTextBoxDefault, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy())));
                resetSelection();
            }
        } else if (state == ACTIVE) {
            if (key == (int)InputDeviceKey::LEFT_ARROW) {
                if (cursorIndex > 0) {
                    cursorIndex--;
                    refreshText(false);
                    resetSelection();
                }
            } else if (key == (int)InputDeviceKey::RIGHT_ARROW) {
                assert(cursorIndex <= originalText.length());
                if (cursorIndex < originalText.length()) {
                    cursorIndex++;
                    refreshText(false);
                    resetSelection();
                }
            } else if (key == (int)InputDeviceKey::BACKSPACE) {
                if (selectionStartIndex != cursorIndex) {
                    deleteSelectedText();
                } else if (cursorIndex > 0) {
                    U32StringA tmpRight = originalText.substr((unsigned long)cursorIndex, originalText.length() - cursorIndex);
                    originalText = originalText.substr(0, (unsigned long)(cursorIndex - 1L));
                    originalText.append(tmpRight);

                    cursorIndex--;
                    refreshText(true);
                    resetSelection();
                }
            } else if (key == (int)InputDeviceKey::DELETE_KEY) {
                if (selectionStartIndex != cursorIndex) {
                    deleteSelectedText();
                } else if (cursorIndex < originalText.length()) {
                    U32StringA tmpRight = originalText.substr((unsigned long)(cursorIndex + 1L), originalText.length() - cursorIndex);
                    originalText = originalText.substr(0, (unsigned long)cursorIndex);
                    originalText.append(tmpRight);

                    refreshText(true);
                    resetSelection();
                }
            }
        }
        return true;
    }

    bool TextBox::onKeyReleaseEvent(unsigned int key) {
        if (key == (int)InputDeviceKey::MOUSE_LEFT) {
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
                if (selectionStartIndex != cursorIndex) {
                    deleteSelectedText();
                }

                if (!isMaxCharacterReach()) {
                    U32StringA tmpRight = originalText.substr((unsigned long) cursorIndex, originalText.length() - cursorIndex);
                    originalText = originalText.substr(0, (unsigned long) cursorIndex);
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
            cursorPosition = computeCursorPosition(cursorIndex);
            displaySelection();
            return false;
        }
        return true;
    }

    void TextBox::onResetStateEvent() {
        state = INACTIVE;
        textBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texTextBoxDefault, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy())));
    }

    bool TextBox::isCharacterAllowed(char32_t unicodeCharacter) const {
        return allowedCharacters.empty() || std::ranges::find(allowedCharacters, unicodeCharacter) != allowedCharacters.end();
    }

    bool TextBox::isMaxCharacterReach() const {
        return maxCharacter != -1 && (int)originalText.size() >= maxCharacter;
    }

    void TextBox::refreshText(bool textUpdated) {
        //refresh start index
        cursorPosition = computeCursorPosition(cursorIndex);
        if (cursorPosition.X > (int)maxWidthText) {
            startTextIndex = (startTextIndex <= originalText.length()) ? startTextIndex + TextFieldConst::LETTER_SHIFT : (unsigned int)originalText.length();
            startTextIndex = std::min(startTextIndex, (unsigned int)originalText.length());
        } else if (cursorIndex <= startTextIndex) {
            startTextIndex = (startTextIndex >= TextFieldConst::LETTER_SHIFT) ? startTextIndex - TextFieldConst::LETTER_SHIFT : 0;
        }
        cursorPosition = computeCursorPosition(cursorIndex);

        //determine the text to display
        const auto& font = text->getFont();
        unsigned int widthText = 0;
        unsigned int endTextIndex = startTextIndex;
        for (; endTextIndex < originalText.length(); ++endTextIndex) {
            char32_t textLetter = originalText[endTextIndex];
            widthText += font.getGlyph(textLetter).width + font.getSpaceBetweenLetters();
            if (widthText > maxWidthText) {
                break;
            }
        }
        U32StringA textToDisplay = originalText.substr((unsigned long)startTextIndex, (unsigned long)(endTextIndex - startTextIndex));
        text->updateText(std::string(stringConvert.to_bytes(textToDisplay)));

        //event
        if (textUpdated) {
            for (auto& eventListener : getEventListeners()) {
                eventListener->onValueChange(this);
            }
        }
    }

    Point2<int> TextBox::computeCursorPosition(unsigned int cursorIdx) {
        Point2<int> computedCursorPosition(0.0f, 0.0f);

        for (unsigned int i = startTextIndex; i < cursorIdx; ++i) {
            char32_t textLetter = originalText[i];
            computedCursorPosition.X += (int)(text->getFont().getGlyph(textLetter).width + text->getFont().getSpaceBetweenLetters());
        }

        if (computedCursorPosition.X > 0) {
            computedCursorPosition.X -= (int)text->getFont().getSpaceBetweenLetters(); //remove last space
            computedCursorPosition.X += TextFieldConst::LETTER_AND_CURSOR_SHIFT;
        }

        cursorBlink = 0.0f;
        return computedCursorPosition;
    }

    unsigned int TextBox::computeCursorIndex(int approximatePositionX, int) const {
        const auto& font = text->getFont();
        float widthText = 0.0f;

        unsigned int computedCursorIndex = startTextIndex;
        for (; computedCursorIndex < originalText.length(); ++computedCursorIndex) {
            char32_t textLetter = originalText[computedCursorIndex];
            widthText += (float)font.getGlyph(textLetter).width / 2.0f;
            if (widthText > (float)approximatePositionX) {
                break;
            }

            widthText += (float)font.getGlyph(textLetter).width / 2.0f + (float)font.getSpaceBetweenLetters();
        }
        return computedCursorIndex;
    }

    void TextBox::resetSelection() {
        selectionStartIndex = cursorIndex;
        selectionImg->setIsVisible(false);
    }

    void TextBox::displaySelection() {
        selectionImg->setIsVisible(true);

        unsigned int displaySelectionStartIndex = std::min((unsigned int)selectionStartIndex, cursorIndex);
        unsigned int displaySelectionEndIndex = std::max((unsigned int)selectionStartIndex, cursorIndex);

        Point2<int> displaySelectionStartPos = computeCursorPosition(displaySelectionStartIndex) + Point2<int>(0, -(int)TextFieldConst::CURSOR_HEIGHT_MARGIN_PIXEL);
        Point2<int> displaySelectionEndPos = computeCursorPosition(displaySelectionEndIndex) + Point2<int>(0, (int)text->getFont().getHeight() + (int)TextFieldConst::CURSOR_HEIGHT_MARGIN_PIXEL * 2);

        selectionImg->updatePosition(Position((float)displaySelectionStartPos.X, (float)displaySelectionStartPos.Y, PIXEL, PARENT_LEFT_CENTERY, RefPoint::LEFT_CENTERY));
        selectionImg->updateSize(Size((float)(displaySelectionEndPos.X - displaySelectionStartPos.X), (float)(displaySelectionEndPos.Y - displaySelectionStartPos.Y), PIXEL));
    }

    void TextBox::deleteSelectedText() {
        U32StringA tmpRight = originalText.substr(std::max(selectionStartIndex, (std::size_t)cursorIndex), originalText.length() - std::max(selectionStartIndex, (std::size_t)cursorIndex));
        originalText = originalText.substr(0, std::min(selectionStartIndex, (std::size_t)cursorIndex));
        originalText.append(tmpRight);
        refreshText(true);

        cursorIndex = std::min((unsigned int)selectionStartIndex, cursorIndex);
        cursorPosition = computeCursorPosition(cursorIndex);
        resetSelection();
    }

    void TextBox::prepareWidgetRendering(float dt, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        //text box
        updateProperties(textBoxRenderer.get(), projectionViewMatrix, Vector2<float>(getGlobalPositionX(), getGlobalPositionY()));
        textBoxRenderer->enableRenderer(renderingOrder);

        //cursor
        cursorBlink += dt * TextFieldConst::CURSOR_BLINK_SPEED;
        if (state == ACTIVE) {
            if (((int)cursorBlink % 2) == 0) {
                cursor->updatePosition(Position((float)cursorPosition.X, (float)cursorPosition.Y - (float)TextFieldConst::CURSOR_HEIGHT_MARGIN_PIXEL, PIXEL, PARENT_LEFT_CENTERY, RefPoint::LEFT_CENTERY));
                cursor->setIsVisible(true);
            } else {
                cursor->setIsVisible(false);
            }
        }
    }

}
