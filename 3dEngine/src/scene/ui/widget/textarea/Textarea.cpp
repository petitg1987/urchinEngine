#include <scene/ui/widget/textarea/Textarea.h>
#include <scene/InputDeviceKey.h>

namespace urchin {

    Textarea::Textarea(Position position, Size size, std::string skinName) :
            Widget(position, size),
            skinName(std::move(skinName)),
            maxCharacter(-1),
            textCursorIndex({0, 0}),
            cursorBlink(0.0f),
            state(INACTIVE) {
        originalTextLines.emplace_back();
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

    void Textarea::updateText(std::string_view textValue) {
        originalTextLines.clear();
        std::vector<std::string> lines = StringUtil::split(textValue, '\n');
        if (!lines.empty()) {
            for (const std::string& line: lines) {
                originalTextLines.emplace_back(line.begin(), line.end());
            }
        } else {
            originalTextLines.emplace_back();
        }
        refreshText();

        textCursorIndex.line = text->getCutTextLines().size() - 1;
        textCursorIndex.column = text->getCutTextLines()[textCursorIndex.line].text.length();
        refreshCursorPosition();
    }

    void Textarea::createOrUpdateWidget() {
        //delete children
        detachChild(text.get());
        //TODO add container

        //skin information
        auto textareaChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textarea", UdaAttribute("skin", skinName));

        auto skinChunkDefault = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "default"), textareaChunk);
        texTextareaDefault = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunkDefault, &widgetOutline);

        auto skinChunkFocus = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "focus"), textareaChunk);
        texTextareaFocus = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunkFocus);

        auto textSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textSkin", UdaAttribute(), textareaChunk);
        text = Text::create(this, Position(0.0f, 0.0f, LengthType::PIXEL), textSkinChunk->getStringValue(), "");
        float maxWidthText = getWidth() - (float)(widgetOutline.leftWidth + widgetOutline.rightWidth);
        text->setMaxWidth(maxWidthText, PIXEL);

        Vector3<float> fontColor = text->getFont().getFontColor();
        std::vector<unsigned char> cursorColor = {static_cast<unsigned char>(fontColor.X * 255), static_cast<unsigned char>(fontColor.Y * 255), static_cast<unsigned char>(fontColor.Z * 255), 255};
        texCursorAlbedo = Texture::build("cursor albedo", 1, 1, TextureFormat::RGBA_8_INT, cursorColor.data());
        textCursorIndex.line = 0;
        textCursorIndex.column = 0;
        refreshText();

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
                if (textCursorIndex.column > 0) {
                    textCursorIndex.column -= 1;
                    refreshCursorPosition();
                } else if (textCursorIndex.column == 0 && textCursorIndex.line > 0) {
                    textCursorIndex.line -= 1;
                    textCursorIndex.column = text->getCutTextLines()[textCursorIndex.line].text.length();
                    refreshCursorPosition();
                }
                refreshCursorPosition();
            } else if (key == (int)InputDeviceKey::RIGHT_ARROW) {
                if (textCursorIndex.column < text->getCutTextLines()[textCursorIndex.line].text.length()) {
                    textCursorIndex.column += 1;
                    refreshCursorPosition();
                } else if (textCursorIndex.line < text->getCutTextLines().size() - 1) {
                    textCursorIndex.line += 1;
                    textCursorIndex.column = 0;
                    refreshCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::UP_ARROW) {
                if (textCursorIndex.line > 0) {
                    computeCursorIndex(cursorPosition.X, cursorPosition.Y - (int)text->getFont().getSpaceBetweenLines());
                    refreshCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::DOWN_ARROW) {
                if (textCursorIndex.line < text->getCutTextLines().size() - 1) {
                    computeCursorIndex(cursorPosition.X, cursorPosition.Y + (int)text->getFont().getSpaceBetweenLines());
                    refreshCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::BACKSPACE) {
                CursorIndex originalTextCursorIndex = textCursorIndexToTextOriginal(textCursorIndex);
                if (originalTextCursorIndex.column > 0) {
                    U32StringA& currentLine = originalTextLines[originalTextCursorIndex.line];
                    U32StringA tmpLeft = currentLine.substr(0, (originalTextCursorIndex.column - 1L));
                    U32StringA tmpRight = currentLine.substr(originalTextCursorIndex.column, currentLine.length() - originalTextCursorIndex.column);
                    currentLine = tmpLeft + tmpRight;
                    refreshText();

                    originalTextCursorIndex.column -= 1;
                    textCursorIndex = textOriginalCursorIndexToText(originalTextCursorIndex);
                    refreshCursorPosition();
                } else if (originalTextCursorIndex.column == 0 && originalTextCursorIndex.line > 0) {
                    std::size_t previousLineLength = originalTextLines[originalTextCursorIndex.line - 1].length();
                    originalTextLines[originalTextCursorIndex.line - 1] += originalTextLines[originalTextCursorIndex.line];
                    originalTextLines.erase(originalTextLines.begin() + (long)originalTextCursorIndex.line);
                    refreshText();

                    originalTextCursorIndex.line -= 1;
                    originalTextCursorIndex.column = previousLineLength;
                    textCursorIndex = textOriginalCursorIndexToText(originalTextCursorIndex);
                    refreshCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::DELETE_KEY) {
                CursorIndex originalTextCursorIndex = textCursorIndexToTextOriginal(textCursorIndex);
                if (originalTextCursorIndex.column < originalTextLines[originalTextCursorIndex.line].length()) {
                    U32StringA& currentLine = originalTextLines[originalTextCursorIndex.line];
                    U32StringA tmpLeft = currentLine.substr(0, originalTextCursorIndex.column);
                    U32StringA tmpRight = currentLine.substr(originalTextCursorIndex.column + 1L, currentLine.length() - originalTextCursorIndex.column);
                    currentLine = tmpLeft + tmpRight;
                    refreshText();

                    textCursorIndex = textOriginalCursorIndexToText(originalTextCursorIndex);
                    refreshCursorPosition();
                } else if (originalTextCursorIndex.line < originalTextLines.size() - 1) {
                    originalTextLines[originalTextCursorIndex.line] += originalTextLines[originalTextCursorIndex.line + 1];
                    originalTextLines.erase(originalTextLines.begin() + (long)originalTextCursorIndex.line + 1);
                    refreshText();

                    textCursorIndex = textOriginalCursorIndexToText(originalTextCursorIndex);
                    refreshCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::ENTER || key == (int)InputDeviceKey::NUM_PAD_ENTER) {
                CursorIndex originalTextCursorIndex = textCursorIndexToTextOriginal(textCursorIndex);
                U32StringA& currentLine = originalTextLines[originalTextCursorIndex.line];
                U32StringA tmpLeft = currentLine.substr(0, originalTextCursorIndex.column);
                U32StringA tmpRight = currentLine.substr(originalTextCursorIndex.column, currentLine.length() - originalTextCursorIndex.column);
                currentLine = tmpLeft;
                originalTextLines.insert(originalTextLines.begin() + (long)originalTextCursorIndex.line + 1, tmpRight);
                refreshText();

                originalTextCursorIndex.line += 1;
                originalTextCursorIndex.column = 0;
                textCursorIndex = textOriginalCursorIndexToText(originalTextCursorIndex);
                refreshCursorPosition();
            }
        }

        return true;
    }

    bool Textarea::onCharEvent(char32_t unicodeCharacter) {
        if (state == ACTIVE) {
            if (isCharacterAllowed(unicodeCharacter) && !isMaxCharacterReach()) {
                CursorIndex originalTextCursorIndex = textCursorIndexToTextOriginal(textCursorIndex);
                U32StringA& currentLine = originalTextLines[originalTextCursorIndex.line];
                U32StringA tmpLeft = currentLine.substr(0, originalTextCursorIndex.column);
                U32StringA tmpRight = currentLine.substr(originalTextCursorIndex.column, currentLine.length() - originalTextCursorIndex.column);
                currentLine = tmpLeft + unicodeCharacter + tmpRight;
                refreshText();

                originalTextCursorIndex.column += 1;
                textCursorIndex = textOriginalCursorIndexToText(originalTextCursorIndex);
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

        int totalCharacters = 0;
        for (const U32StringA& originalTextLine : originalTextLines) {
            totalCharacters += (int)originalTextLine.length();
        }
        return totalCharacters >= maxCharacter;
    }

    void Textarea::refreshText() {
        //refresh text
        std::vector<std::string> lines;
        lines.reserve(originalTextLines.size());
        for (const U32StringA& originalTextLine : originalTextLines) {
            lines.emplace_back(stringConvert.to_bytes(originalTextLine));
        }
        text->updateText(StringUtil::merge(lines, '\n'));
    }

    void Textarea::refreshCursorPosition() {
        cursorPosition.X = 0;
        cursorPosition.Y = (int)(text->getFont().getSpaceBetweenLines() * textCursorIndex.line);

        const U32StringA& currentLine = text->getCutTextLines()[(std::size_t)textCursorIndex.line].text;
        for (std::size_t i = 0; i < (std::size_t)textCursorIndex.column; ++i) {
            char32_t textLetter = currentLine[i];
            cursorPosition.X += (int)(text->getFont().getGlyph(textLetter).width + text->getFont().getSpaceBetweenLetters());
        }

        if (cursorPosition.X > 0) {
            cursorPosition.X -= (int)text->getFont().getSpaceBetweenLetters(); //remove last space
            cursorPosition.X += LETTER_AND_CURSOR_SHIFT;
        }

        cursorPosition.X += widgetOutline.leftWidth;
        cursorPosition.Y += widgetOutline.topWidth;
    }

    CursorIndex Textarea::textOriginalCursorIndexToText(const CursorIndex& originalCursorIndex) const { //TODO add unit test
        CursorIndex textCursorIndex = {.line = 0, .column = 0};
        for (std::size_t lineIndex = 0; lineIndex < originalTextLines.size(); ++lineIndex) {
            if (lineIndex == originalCursorIndex.line) {
                std::size_t remainingColumnShift = originalCursorIndex.column;
                while (true) {
                    if (text->getCutTextLines()[textCursorIndex.line].text.length() >= remainingColumnShift) {
                        textCursorIndex.column = remainingColumnShift;
                        return textCursorIndex;
                    } else {
                        remainingColumnShift -= text->getCutTextLines()[textCursorIndex.line].text.length();
                        if (text->getCutTextLines()[textCursorIndex.line].spaceIntoLineFeed) {
                            remainingColumnShift -= 1;
                        }
                        textCursorIndex.line++;
                    }
                }
            } else {
                std::size_t lineCharactersCount = originalTextLines[lineIndex].length();
                std::size_t textLineCharactersCount = 0;
                while (true) {
                    textLineCharactersCount += text->getCutTextLines()[textCursorIndex.line].text.length();
                    textCursorIndex.line++;

                    if (textLineCharactersCount >= lineCharactersCount) {
                        assert(textLineCharactersCount == lineCharactersCount);
                        break;
                    } else {
                        if (text->getCutTextLines()[textCursorIndex.line].spaceIntoLineFeed) {
                            textLineCharactersCount += 1;
                        }
                    }
                }
            }
        }
        return textCursorIndex;
    }

    CursorIndex Textarea::textCursorIndexToTextOriginal(const CursorIndex& textCursorIndex) const { //TODO avoid brute force + add unit test
        CursorIndex originalCursorIndex = {.line = 0, .column = 0};
        while (true) {
            for (std::size_t lineIndex = 0; lineIndex < originalTextLines.size(); ++lineIndex) {
                originalCursorIndex.line = lineIndex;

                for (std::size_t columnIndex = 0; columnIndex <= originalTextLines[lineIndex].length(); ++columnIndex) {
                    originalCursorIndex.column = columnIndex;

                    CursorIndex computedTextCursorIndex = textOriginalCursorIndexToText(originalCursorIndex);
                    if (computedTextCursorIndex.line == textCursorIndex.line && computedTextCursorIndex.column == textCursorIndex.column) {
                        return originalCursorIndex;
                    }
                }
            }
        }
    }

    void Textarea::computeCursorIndex(int approximatePositionX, int approximatePositionY) {
        textCursorIndex.line = (std::size_t)((float)approximatePositionY / (float) text->getFont().getSpaceBetweenLines());
        textCursorIndex.line = std::min(textCursorIndex.line, text->getCutTextLines().size() - 1);

        textCursorIndex.column = 0;
        float currentWidth = 0.0f;
        for (textCursorIndex.column = 0; textCursorIndex.column < text->getCutTextLines()[textCursorIndex.line].text.length(); ++textCursorIndex.column) {
            char32_t textLetter = text->getCutTextLines()[textCursorIndex.line].text[textCursorIndex.column];

            currentWidth += (float)text->getFont().getGlyph(textLetter).width / 2.0f;
            if ((float)approximatePositionX <= currentWidth) {
                break;
            }
            currentWidth += (float)text->getFont().getGlyph(textLetter).width / 2.0f + (float) text->getFont().getSpaceBetweenLetters();
        }

        //compute the exact cursor position
        refreshCursorPosition();
    }

    void Textarea::prepareWidgetRendering(float dt, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        //text area
        updateProperties(textareaRenderer.get(), projectionViewMatrix, Vector2<float>(getGlobalPositionX(), getGlobalPositionY()));
        textareaRenderer->enableRenderer(renderingOrder);

        //cursor
        cursorBlink += dt * CURSOR_BLINK_SPEED;
        if (state == ACTIVE && ((int)cursorBlink % 2) > 0) {
            renderingOrder++;
            updateProperties(cursorRenderer.get(), projectionViewMatrix, Vector2<float>(getGlobalPositionX() + (float)cursorPosition.X, getGlobalPositionY() + (float)cursorPosition.Y));
            cursorRenderer->enableRenderer(renderingOrder);
        }
    }

}
