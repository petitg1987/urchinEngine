#include <scene/ui/widget/textarea/Textarea.h>
#include <scene/InputDeviceKey.h>

namespace urchin {

    Textarea::Textarea(Position position, Size size, std::string skinName) :
            Widget(position, size),
            skinName(std::move(skinName)),
            maxCharacter(-1),
            cursorIndex({0, 0}),
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

        cursorIndex.line = (int)originalTextLines.size() - 1;
        cursorIndex.column = (int)originalTextLines[(std::size_t)cursorIndex.line].length();

        refreshText();
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
        cursorIndex.line = 0;
        cursorIndex.column = 0;
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
                if (cursorIndex.column > 0) {
                    cursorIndex.column -= 1;
                    computeCursorPosition();
                } else if (cursorIndex.column == 0 && cursorIndex.line > 0) {
                    cursorIndex.line -= 1;
                    cursorIndex.column = (int)originalTextLines[(std::size_t)cursorIndex.line].length();
                    computeCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::RIGHT_ARROW) {
                if (cursorIndex.column < (int)originalTextLines[(std::size_t)cursorIndex.line].length()) {
                    cursorIndex.column += 1;
                    computeCursorPosition();
                } else if (cursorIndex.line < (int)originalTextLines.size() - 1) {
                    cursorIndex.line += 1;
                    cursorIndex.column = 0;
                    computeCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::UP_ARROW) {
                if (cursorIndex.line > 0) {
                    computeCursorIndex(cursorPosition.X, cursorPosition.Y - (int)text->getFont().getSpaceBetweenLines());
                    computeCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::DOWN_ARROW) {
                if (cursorIndex.line < (int)originalTextLines.size() - 1) {
                    computeCursorIndex(cursorPosition.X, cursorPosition.Y + (int)text->getFont().getSpaceBetweenLines());
                    computeCursorPosition();
                }
            } else if (key == (int)InputDeviceKey::BACKSPACE) {
                if (cursorIndex.column > 0) {
                    U32StringA& currentLine = originalTextLines[(std::size_t)cursorIndex.line];
                    U32StringA tmpLeft = currentLine.substr(0, (unsigned long)(cursorIndex.column - 1L));
                    U32StringA tmpRight = currentLine.substr((unsigned long)cursorIndex.column, currentLine.length() - (unsigned long)cursorIndex.column);
                    currentLine = tmpLeft + tmpRight;

                    cursorIndex.column -= 1;
                    refreshText();
                } else if (cursorIndex.column == 0 && cursorIndex.line > 0) {
                    std::size_t previousLineLength = originalTextLines[(std::size_t)cursorIndex.line - 1].length();
                    originalTextLines[(std::size_t)cursorIndex.line - 1] += originalTextLines[(std::size_t)cursorIndex.line];
                    originalTextLines.erase(originalTextLines.begin() + cursorIndex.line);

                    cursorIndex.line -= 1;
                    cursorIndex.column = (int)previousLineLength;
                    refreshText();
                }
            } else if (key == (int)InputDeviceKey::DELETE_KEY) {
                if (cursorIndex.column < (int)originalTextLines[(std::size_t)cursorIndex.line].length()) {
                    U32StringA& currentLine = originalTextLines[(std::size_t)cursorIndex.line];
                    U32StringA tmpLeft = currentLine.substr(0, (unsigned long)cursorIndex.column);
                    U32StringA tmpRight = currentLine.substr((unsigned long)cursorIndex.column + 1L, currentLine.length() - (unsigned long)cursorIndex.column);
                    currentLine = tmpLeft + tmpRight;

                    refreshText();
                } else if (cursorIndex.line < (int)originalTextLines.size() - 1) {
                    originalTextLines[(std::size_t)cursorIndex.line] += originalTextLines[(std::size_t)cursorIndex.line + 1];
                    originalTextLines.erase(originalTextLines.begin() + cursorIndex.line + 1);

                    refreshText();
                }
            } else if (key == (int)InputDeviceKey::ENTER || key == (int)InputDeviceKey::NUM_PAD_ENTER) {
                U32StringA& currentLine = originalTextLines[(std::size_t)cursorIndex.line];
                U32StringA tmpLeft = currentLine.substr(0, (unsigned long)cursorIndex.column);
                U32StringA tmpRight = currentLine.substr((unsigned long)cursorIndex.column, currentLine.length() - (unsigned long)cursorIndex.column);
                currentLine = tmpLeft;
                originalTextLines.insert(originalTextLines.begin() + cursorIndex.line + 1, tmpRight);

                cursorIndex.line += 1;
                cursorIndex.column = 0;
                refreshText();
            }
        }

        return true;
    }

    bool Textarea::onCharEvent(char32_t unicodeCharacter) {
        if (state == ACTIVE) {
            if (isCharacterAllowed(unicodeCharacter) && !isMaxCharacterReach()) {
                U32StringA& currentLine = originalTextLines[(std::size_t)cursorIndex.line];
                U32StringA tmpLeft = currentLine.substr(0, (unsigned long)cursorIndex.column);
                U32StringA tmpRight = currentLine.substr((unsigned long)cursorIndex.column, currentLine.length() - (unsigned long)cursorIndex.column);
                currentLine = tmpLeft + unicodeCharacter + tmpRight;

                cursorIndex.column += 1;
                refreshText();
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

        //re-compute cursor position
        computeCursorPosition();
    }

    void Textarea::computeCursorPosition() { //TODO convert cursorIndex in originalTextLines to text
        cursorPosition.X = 0;
        cursorPosition.Y = (int)text->getFont().getSpaceBetweenLines() * cursorIndex.line;

        const U32StringA& currentLine = originalTextLines[(std::size_t)cursorIndex.line];
        for (std::size_t i = 0; i < (std::size_t)cursorIndex.column; ++i) {
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

    void Textarea::computeCursorIndex(int approximatePositionX, int approximatePositionY) {
        cursorIndex.line = (int)((float)approximatePositionY / (float) text->getFont().getSpaceBetweenLines());
        cursorIndex.line = std::min(cursorIndex.line, (int)originalTextLines.size() - 1);

        cursorIndex.column = 0;
        float currentWidth = 0.0f;
        for (cursorIndex.column = 0; cursorIndex.column < (int)originalTextLines[(std::size_t)cursorIndex.line].length(); ++cursorIndex.column) {
            char32_t textLetter = originalTextLines[(std::size_t)cursorIndex.line][(std::size_t)cursorIndex.column];

            currentWidth += (float)text->getFont().getGlyph(textLetter).width / 2.0f;
            if ((float) approximatePositionX < currentWidth) {
                break;
            }
            currentWidth += (float)text->getFont().getGlyph(textLetter).width / 2.0f + (float) text->getFont().getSpaceBetweenLetters();
        }

        //compute the exact cursor position
        computeCursorPosition();
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
