#include <scene/ui/widget/textarea/Textarea.h>
#include <scene/ui/widget/TextFieldConst.h>
#include <scene/InputDeviceKey.h>

namespace urchin {

    Textarea::Textarea(Position position, Size size, std::string skinName) :
            Widget(position, size),
            skinName(std::move(skinName)),
            maxCharacter(-1),
            scrollbarWidthInPixel(0.0f),
            cursorIndex(0),
            cursorBlink(0.0f),
            state(INACTIVE) {

    }

    std::shared_ptr<Textarea> Textarea::create(Widget* parent, Position position, Size size, std::string skinName) {
        return Widget::create<Textarea>(new Textarea(position, size, std::move(skinName)), parent);
    }

    WidgetType Textarea::getWidgetType() const {
        return WidgetType::TEXTAREA;
    }

    std::string Textarea::getText() const {
        return text->getBaseText();
    }

    void Textarea::updateText(std::string_view text) {
        this->originalText = U32StringA(text.begin(), text.end());
        refreshText(true);

        this->cursorIndex = originalText.length();
        refreshCursorPosition();
    }

    void Textarea::createOrUpdateWidget() {
        //delete children
        detachChild(textContainer.get());

        //skin information
        auto textareaChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textarea", UdaAttribute("skin", skinName));

        auto skinChunkDefault = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "default"), textareaChunk);
        texTextareaDefault = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunkDefault, &widgetOutline);

        auto skinChunkFocus = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "focus"), textareaChunk);
        texTextareaFocus = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunkFocus);

        auto scrollbarSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "scrollbarSkin", UdaAttribute(), textareaChunk);
        std::string scrollbarSkinName = scrollbarSkinChunk->getStringValue();
        auto scrollbarChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "scrollbar", UdaAttribute("skin", scrollbarSkinName));
        Length scrollbarWidth = UISkinService::instance().loadLength(scrollbarChunk, "width");
        scrollbarWidthInPixel = widthLengthToPixel(scrollbarWidth.value, scrollbarWidth.type, [](){return 0.0f;}) + 4;
        textContainer = Container::createScrollable(this, Position(0.0f, 0.0f, SCREEN_PERCENT), Size(100.0f, 100.0f, PARENT_PERCENT), scrollbarSkinName);

        auto textSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textSkin", UdaAttribute(), textareaChunk);
        text = Text::create(textContainer.get(), Position(0.0f, 0.0f, PIXEL), textSkinChunk->getStringValue(), "");
        float maxWidthText = getWidth() - (float)(widgetOutline.leftWidth + widgetOutline.rightWidth) - scrollbarWidthInPixel;
        text->setMaxWidth(maxWidthText, PIXEL);

        Vector3<float> fontColor = text->getFont().getFontColor();
        std::vector<unsigned char> cursorColor = {static_cast<unsigned char>(fontColor.X * 255), static_cast<unsigned char>(fontColor.Y * 255), static_cast<unsigned char>(fontColor.Z * 255), 255};
        texCursorAlbedo = Texture::build("cursor albedo", 1, 1, TextureFormat::RGBA_8_INT, cursorColor.data());
        Size cursorSize(TextFieldConst::CURSOR_WIDTH_PIXEL, (float)text->getFont().getHeight() + ((float)TextFieldConst::CURSOR_HEIGHT_MARGIN_PIXEL * 2.0f), PIXEL);
        cursor = StaticBitmap::create(textContainer.get(), Position(0.0f, 0.0f, PIXEL), cursorSize, texCursorAlbedo);
        cursor->setIsVisible(false);

        refreshText(false);
        refreshCursorPosition();

        //visual
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(getWidth(), 0.0f), Point2<float>(getWidth(), getHeight()),
                Point2<float>(0.0f, 0.0f), Point2<float>(getWidth(), getHeight()), Point2<float>(0.0f, getHeight())
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        textareaRenderer = setupUiRenderer("textarea", ShapeType::TRIANGLE, false)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(texTextareaDefault, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy()))) //binding 3
                ->build();
    }


    bool Textarea::onKeyPressEvent(unsigned int key) {
        if (key == (int)InputDeviceKey::MOUSE_LEFT) {
            if (widgetRectangle().collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                state = ACTIVE;
                textareaRenderer->updateUniformTextureReader(0, TextureReader::build(texTextareaFocus, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy())));

                Rectangle2D textZone(
                        Point2<int>((int)getGlobalPositionX(), (int)getGlobalPositionY()),
                        Point2<int>((int)getGlobalPositionX() + (int)getWidth() - (int)scrollbarWidthInPixel, (int)getGlobalPositionY() + (int)getHeight()));
                if (textZone.collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                    int localMouseX = getMouseX() - MathFunction::roundToInt(text->getGlobalPositionX());
                    int localMouseY = getMouseY() - MathFunction::roundToInt(text->getGlobalPositionY());
                    computeCursorIndex(localMouseX, localMouseY);
                }
            } else {
                state = INACTIVE;
                textareaRenderer->updateUniformTextureReader(0, TextureReader::build(texTextareaDefault, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy())));
            }
        } else if (state == ACTIVE) {
            if (key == (int)InputDeviceKey::LEFT_ARROW) {
                if (cursorIndex > 0) {
                    cursorIndex--;
                    refreshCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::RIGHT_ARROW) {
                assert(cursorIndex <= originalText.length());
                if (cursorIndex < originalText.length()) {
                    cursorIndex++;
                    refreshCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::UP_ARROW) {
                computeCursorIndex(cursorPosition.X, cursorPosition.Y - (int)text->getFont().getSpaceBetweenLines());
                refreshCursorPosition();
            } else if (key == (int)InputDeviceKey::DOWN_ARROW) {
                computeCursorIndex(cursorPosition.X, cursorPosition.Y + (int)text->getFont().getSpaceBetweenLines());
                refreshCursorPosition();
            } else if (key == (int)InputDeviceKey::BACKSPACE) {
                if (cursorIndex > 0) {
                    U32StringA tmpRight = originalText.substr(cursorIndex, originalText.length() - cursorIndex);
                    originalText = originalText.substr(0, cursorIndex - 1L);
                    originalText.append(tmpRight);
                    refreshText(true);

                    cursorIndex--;
                    refreshCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::DELETE_KEY) {
                if (cursorIndex < originalText.length()) {
                    U32StringA tmpRight = originalText.substr(cursorIndex + 1L, originalText.length() - cursorIndex);
                    originalText = originalText.substr(0, cursorIndex);
                    originalText.append(tmpRight);
                    refreshText(true);

                    refreshCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::ENTER || key == (int)InputDeviceKey::NUM_PAD_ENTER) {
                U32StringA tmpRight = originalText.substr(cursorIndex, originalText.length() - cursorIndex);
                originalText = originalText.substr(0, cursorIndex);
                originalText.append(1, '\n');
                originalText.append(tmpRight);
                refreshText(true);

                cursorIndex++;
                refreshCursorPosition();
            }
        }
        return true;
    }

    bool Textarea::onCharEvent(char32_t unicodeCharacter) {
        if (state == ACTIVE) {
            if (isCharacterAllowed(unicodeCharacter) && !isMaxCharacterReach()) {
                U32StringA tmpRight = originalText.substr(cursorIndex, originalText.length() - cursorIndex);
                originalText = originalText.substr(0, cursorIndex);
                originalText.append(1, unicodeCharacter);
                originalText.append(tmpRight);
                refreshText(true);

                cursorIndex++;
                refreshCursorPosition();
            }
            return false;
        }
        return true;
    }

    void Textarea::onResetStateEvent() {
        state = INACTIVE;
        textareaRenderer->updateUniformTextureReader(0, TextureReader::build(texTextareaDefault, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy())));
    }

    bool Textarea::isCharacterAllowed(char32_t unicodeCharacter) const {
        return allowedCharacters.empty() || std::ranges::find(allowedCharacters, unicodeCharacter) != allowedCharacters.end();
    }

    bool Textarea::isMaxCharacterReach() const {
        if (maxCharacter == -1) {
            return false;
        }
        return (int)originalText.length() >= maxCharacter;
    }

    void Textarea::refreshText(bool textUpdated) {
        text->updateText(std::string(stringConvert.to_bytes(originalText)));
        textContainer->notifyChildrenUpdated(); //ugly

        if (textUpdated) {
            for (auto& eventListener : getEventListeners()) {
                eventListener->onValueChange(this);
            }
        }
    }

    void Textarea::refreshCursorPosition() {
        std::size_t textCursorIndex = text->baseTextToCutTextIndex(cursorIndex);
        std::size_t currentIndex = 0;

        cursorPosition.Y = 0.0f;
        for (const TextLine& textLine : text->getCutTextLines()) {
            cursorPosition.X = 0.0f;
            for (std::size_t columnIndex = 0; columnIndex <= textLine.text.length(); ++columnIndex) {
                if (currentIndex == textCursorIndex) {
                    if (cursorPosition.X > 0) {
                        cursorPosition.X -= (int)text->getFont().getSpaceBetweenLetters(); //remove last space
                        cursorPosition.X += TextFieldConst::LETTER_AND_CURSOR_SHIFT;
                    }
                    adjustScrollToCursor();
                    cursorBlink = 0.0f;
                    return;
                } else {
                    char32_t textLetter = textLine.text[columnIndex];
                    cursorPosition.X += (int)(text->getFont().getGlyph(textLetter).width + text->getFont().getSpaceBetweenLetters());

                    currentIndex++;
                }
            }
            cursorPosition.Y += (int)text->getFont().getSpaceBetweenLines();
        }
        throw std::runtime_error("Cursor position not found at index " + std::to_string(textCursorIndex) + " for text: " + std::string(stringConvert.to_bytes(originalText)));
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

    void Textarea::computeCursorIndex(int approximatePositionX, int approximatePositionY) {
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
                    currentWidth += (float)text->getFont().getGlyph(textLetter).width / 2.0f;
                    if ((float)approximatePositionX < currentWidth) {
                        break;
                    }
                    currentWidth += (float)text->getFont().getGlyph(textLetter).width / 2.0f + (float) text->getFont().getSpaceBetweenLetters();
                }
                break;
            } else {
                textCursorIndex += text->getCutTextLines()[lineIndex].text.length(); //index at end of line
            }
        }

        cursorIndex = text->cutTextToBaseTextIndex(textCursorIndex);
        refreshCursorPosition();
    }

    void Textarea::prepareWidgetRendering(float dt, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        //text area
        updateProperties(textareaRenderer.get(), projectionViewMatrix, Vector2<float>(getGlobalPositionX(), getGlobalPositionY()));
        textareaRenderer->enableRenderer(renderingOrder);

        //cursor
        cursorBlink += dt * TextFieldConst::CURSOR_BLINK_SPEED;
        if (state == ACTIVE) {
            if (((int)cursorBlink % 2) == 0) {
                cursor->updatePosition(Position((float)cursorPosition.X, (float)cursorPosition.Y - (float)TextFieldConst::CURSOR_HEIGHT_MARGIN_PIXEL, PIXEL));
                cursor->setIsVisible(true);
            } else {
                cursor->setIsVisible(false);
            }
        }
    }

}
