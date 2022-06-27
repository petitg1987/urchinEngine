#include <scene/ui/widget/textarea/Textarea.h>
#include <scene/InputDeviceKey.h>

namespace urchin {

    Textarea::Textarea(Position position, Size size, std::string skinName) :
            Widget(position, size),
            skinName(std::move(skinName)),
            maxCharacter(-1),
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

    std::string Textarea::getText() {
        return text->getText();
    }

    void Textarea::updateText(std::string_view text) {
        this->originalText = U32StringA(text.begin(), text.end());
        refreshText();

        this->cursorIndex = originalText.length();
        refreshCursorPosition();
    }

    void Textarea::createOrUpdateWidget() {
        //delete children
        detachChild(text.get());
        detachChild(textContainer.get());

        //skin information
        auto textareaChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textarea", UdaAttribute("skin", skinName));

        auto skinChunkDefault = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "default"), textareaChunk);
        texTextareaDefault = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunkDefault, &widgetOutline);

        auto skinChunkFocus = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "focus"), textareaChunk);
        texTextareaFocus = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunkFocus);

        auto textSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textSkin", UdaAttribute(), textareaChunk);
        textContainer = Container::create(this, Position(0.0f, 0.0f, SCREEN_PERCENT), Size(100.0f, 100.0, SCREEN_PERCENT));
        text = Text::create(textContainer.get(), Position(0.0f, 0.0f, PIXEL), textSkinChunk->getStringValue(), "");
        float maxWidthText = getWidth() - (float)(widgetOutline.leftWidth + widgetOutline.rightWidth);
        text->setMaxWidth(maxWidthText, PIXEL);

        Vector3<float> fontColor = text->getFont().getFontColor();
        std::vector<unsigned char> cursorColor = {static_cast<unsigned char>(fontColor.X * 255), static_cast<unsigned char>(fontColor.Y * 255), static_cast<unsigned char>(fontColor.Z * 255), 255};
        texCursorAlbedo = Texture::build("cursor albedo", 1, 1, TextureFormat::RGBA_8_INT, cursorColor.data());

        originalText = U32StringA();
        refreshText();

        cursorIndex = 0;
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

        auto cursorStartY = (float)widgetOutline.topWidth - (float)CURSOR_PADDING_PIXEL;
        auto cursorEndY = (float)cursorStartY + (float)text->getFont().getHeight() + ((float)CURSOR_PADDING_PIXEL * 2.0f);
        std::vector<Point2<float>> cursorVertexCoord = {
                Point2<float>(0.0f, cursorStartY), Point2<float>(CURSOR_WIDTH_PIXEL, cursorStartY), Point2<float>(CURSOR_WIDTH_PIXEL, cursorEndY),
                Point2<float>(0.0f, cursorStartY), Point2<float>(CURSOR_WIDTH_PIXEL, cursorEndY), Point2<float>(0.0f, cursorEndY)
        };
        std::vector<Point2<float>> cursorTextureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        cursorRenderer = setupUiRenderer("textarea - cursor", ShapeType::TRIANGLE, false)
                ->addData(cursorVertexCoord)
                ->addData(cursorTextureCoord)
                ->addUniformTextureReader(TextureReader::build(texCursorAlbedo, TextureParam::build(TextureParam::REPEAT, TextureParam::NEAREST, getTextureAnisotropy()))) //binding 3
                ->build();
    }


    bool Textarea::onKeyPressEvent(unsigned int key) {
        if (key == (int)InputDeviceKey::MOUSE_LEFT) {
            if (widgetRectangle().collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                state = ACTIVE;
                textareaRenderer->updateUniformTextureReader(0, TextureReader::build(texTextareaFocus, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy())));

                int localMouseX = getMouseX() - MathFunction::roundToInt(text->getGlobalPositionX());
                int localMouseY = getMouseY() - MathFunction::roundToInt(text->getGlobalPositionY());
                computeCursorIndex(localMouseX, localMouseY);
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
                    U32StringA tmpLeft = originalText.substr(0, cursorIndex - 1L);
                    U32StringA tmpRight = originalText.substr(cursorIndex, originalText.length() - cursorIndex);
                    originalText = tmpLeft + tmpRight;
                    refreshText();

                    cursorIndex--;
                    refreshCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::DELETE_KEY) {
                if (cursorIndex < originalText.length()) {
                    U32StringA tmpLeft = originalText.substr(0, cursorIndex);
                    U32StringA tmpRight = originalText.substr(cursorIndex + 1L, originalText.length() - cursorIndex);
                    originalText = tmpLeft + tmpRight;
                    refreshText();

                    refreshCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::ENTER || key == (int)InputDeviceKey::NUM_PAD_ENTER) {
                U32StringA tmpLeft = originalText.substr(0, cursorIndex);
                U32StringA tmpRight = originalText.substr(cursorIndex, originalText.length() - cursorIndex);
                char32_t lineReturn = '\n';
                originalText = tmpLeft + lineReturn + tmpRight;
                refreshText();

                cursorIndex++;
                refreshCursorPosition();
            }
        }

        return true;
    }

    bool Textarea::onCharEvent(char32_t unicodeCharacter) {
        if (state == ACTIVE) {
            if (isCharacterAllowed(unicodeCharacter) && !isMaxCharacterReach()) {
                U32StringA tmpLeft = originalText.substr(0, cursorIndex);
                U32StringA tmpRight = originalText.substr(cursorIndex, originalText.length() - cursorIndex);
                originalText = tmpLeft + unicodeCharacter + tmpRight;
                refreshText();

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

    void Textarea::refreshText() {
        text->updateText(std::string(stringConvert.to_bytes(originalText)));
    }

    void Textarea::refreshCursorPosition() {
        std::size_t textCursorIndex = cursorIndexToTextCursorIndex(cursorIndex);
        std::size_t currentIndex = 0;

        cursorPosition.Y = 0.0f;
        for (std::size_t lineIndex = 0; lineIndex < text->getCutTextLines().size(); ++lineIndex) {
            cursorPosition.X = 0.0f;
            for (std::size_t columnIndex = 0; columnIndex <= text->getCutTextLines()[lineIndex].text.length(); ++columnIndex) {
                if (currentIndex == textCursorIndex) {
                    if (cursorPosition.X > 0) {
                        cursorPosition.X -= (int)text->getFont().getSpaceBetweenLetters(); //remove last space
                        cursorPosition.X += LETTER_AND_CURSOR_SHIFT;
                    }

                    cursorBlink = 0.0f;
                    return;
                } else {
                    char32_t textLetter = text->getCutTextLines()[lineIndex].text[columnIndex];
                    cursorPosition.X += (int) (text->getFont().getGlyph(textLetter).width + text->getFont().getSpaceBetweenLetters());

                    currentIndex++;
                }
            }
            cursorPosition.Y += (int)text->getFont().getSpaceBetweenLines();
        }
        throw std::runtime_error("Cursor position not found at index " + std::to_string(textCursorIndex) + " for text: " + std::string(stringConvert.to_bytes(originalText)));
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

        cursorIndex = textCursorIndexToCursorIndex(textCursorIndex);
        refreshCursorPosition();
    }

    std::size_t Textarea::textCursorIndexToCursorIndex(std::size_t textCursorIndex) const {
        int delta = 0;
        std::size_t currentIndex = 0;
        for (const TextLine& lineIndex : text->getCutTextLines()) {
            for (std::size_t columnIndex = 0; columnIndex <= lineIndex.text.length(); ++columnIndex) {
                if (currentIndex == textCursorIndex) {
                    return (std::size_t)((long)textCursorIndex + delta);
                } else {
                    currentIndex++;
                }
            }
            if (lineIndex.cutType == TextCutType::MIDDLE_WORD) {
                delta--;
            }
        }
        throw std::runtime_error("Text cursor index " + std::to_string(textCursorIndex) + " does not exist for text: " + std::string(stringConvert.to_bytes(originalText)));
    }

    std::size_t Textarea::cursorIndexToTextCursorIndex(std::size_t cursorIndex) const {
        std::size_t textDelta = 0;
        std::size_t currentTextIndex = 0;
        for (const TextLine& lineIndex : text->getCutTextLines()) {
            for (std::size_t columnIndex = 0; columnIndex <= lineIndex.text.length(); ++columnIndex) {
                if (columnIndex == lineIndex.text.length() && lineIndex.cutType == TextCutType::MIDDLE_WORD) {
                    //cursor cannot be at end of line when cut type is middle of word
                    currentTextIndex++;
                    textDelta++;
                } else if (currentTextIndex == cursorIndex + textDelta) {
                    return currentTextIndex;
                } else {
                    currentTextIndex++;
                }
            }
        }
        throw std::runtime_error("Cursor index " + std::to_string(cursorIndex) + " does not exist for text: " + std::string(stringConvert.to_bytes(originalText)));
    }

    void Textarea::prepareWidgetRendering(float dt, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        //text area
        updateProperties(textareaRenderer.get(), projectionViewMatrix, Vector2<float>(getGlobalPositionX(), getGlobalPositionY()));
        textareaRenderer->enableRenderer(renderingOrder);

        //cursor
        cursorBlink += dt * CURSOR_BLINK_SPEED;
        if (state == ACTIVE && ((int)cursorBlink % 2) == 0) {
            renderingOrder++;
            Vector2<float> cursorTranslate(
                    getGlobalPositionX() + (float)widgetOutline.leftWidth + (float)cursorPosition.X,
                    getGlobalPositionY() + (float)widgetOutline.topWidth + (float)cursorPosition.Y);
            updateProperties(cursorRenderer.get(), projectionViewMatrix, cursorTranslate);
            cursorRenderer->enableRenderer(renderingOrder);
        }
    }

}
