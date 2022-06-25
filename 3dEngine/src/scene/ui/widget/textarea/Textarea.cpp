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
        return std::string(stringConvert.to_bytes(allText));
    }

    void Textarea::updateText(std::string_view text) {
        allText = U32StringA(text.begin(), text.end());
        refreshText((int)allText.length(), true);
    }

    void Textarea::createOrUpdateWidget() {
        //delete children
        detachChild(text.get());

        //skin information
        auto textBoxChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textarea", UdaAttribute("skin", skinName));

        auto skinChunkDefault = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "default"), textBoxChunk);
        texTextareaDefault = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunkDefault, &widgetOutline);

        auto skinChunkFocus = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "focus"), textBoxChunk);
        texTextareaFocus = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunkFocus);

        auto textSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textSkin", UdaAttribute(), textBoxChunk);
        text = Text::create(this, Position(0.0f, 0.0f, LengthType::PIXEL), textSkinChunk->getStringValue(), "");
        text->updatePosition(Position(0.0f, (float)TEXT_SHIFT_Y_PIXEL, LengthType::PIXEL)); //TODO why update pos ?
//TODO        maxWidthText = (unsigned int)((int)getWidth() - (widgetOutline.leftWidth + widgetOutline.rightWidth));

        Vector3<float> fontColor = text->getFont().getFontColor();
        std::vector<unsigned char> cursorColor = {static_cast<unsigned char>(fontColor.X * 255), static_cast<unsigned char>(fontColor.Y * 255), static_cast<unsigned char>(fontColor.Z * 255), 255};
        texCursorAlbedo = Texture::build("cursor albedo", 1, 1, TextureFormat::RGBA_8_INT, cursorColor.data());
        refreshText((int)cursorIndex, false);
        computeCursorPosition();

        //visual
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(getWidth(), 0.0f), Point2<float>(getWidth(), getHeight()),
                Point2<float>(0.0f, 0.0f), Point2<float>(getWidth(), getHeight()), Point2<float>(0.0f, getHeight())
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        textareaRenderer = setupUiRenderer("text box", ShapeType::TRIANGLE, false)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(texTextareaDefault, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy()))) //binding 3
                ->build();

        auto cursorStartY = (float)widgetOutline.topWidth + (float)TEXT_SHIFT_Y_PIXEL - (float)CURSOR_PADDING_PIXEL;
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
                refreshText((int)cursorIndex - 1, false);
            } else if (key == (int)InputDeviceKey::RIGHT_ARROW) {
                refreshText((int)cursorIndex + 1, false);
            } else if (key == (int)InputDeviceKey::UP_ARROW) {
                //TODO impl
            } else if (key == (int)InputDeviceKey::DOWN_ARROW) {
                //TODO impl
            } else if (key == (int)InputDeviceKey::BACKSPACE) {
                if (cursorIndex > 0) {
                    U32StringA tmpRight = allText.substr((unsigned long)cursorIndex, allText.length() - cursorIndex);
                    allText = allText.substr(0, (unsigned long)(cursorIndex - 1L));
                    allText.append(tmpRight);
                    refreshText((int)cursorIndex - 1, true);
                }
            } else if (key == (int)InputDeviceKey::DELETE_KEY) {
                if (allText.length() > 0 && cursorIndex < allText.length()) {
                    U32StringA tmpRight = allText.substr((unsigned long)(cursorIndex + 1L), allText.length() - cursorIndex);
                    allText = allText.substr(0, (unsigned long)cursorIndex);
                    allText.append(tmpRight);
                    refreshText((int)cursorIndex, true);
                }
            } else if (key == (int)InputDeviceKey::ENTER || key == (int)InputDeviceKey::NUM_PAD_ENTER) {
                U32StringA tmpRight = allText.substr((unsigned long)cursorIndex, allText.length() - cursorIndex);
                allText = allText.substr(0, (unsigned long)cursorIndex);
                allText.append(1, '\n');
                allText.append(tmpRight);
                refreshText((int)cursorIndex + 1, true);
            }
        }

        return true;
    }

    bool Textarea::onCharEvent(char32_t unicodeCharacter) {
        if (state == ACTIVE) {
            if (isCharacterAllowed(unicodeCharacter) && !isMaxCharacterReach()) {
                U32StringA tmpRight = allText.substr((unsigned long)cursorIndex, allText.length() - cursorIndex);
                allText = allText.substr(0, (unsigned long)cursorIndex);
                allText.append(1, unicodeCharacter);
                allText.append(tmpRight);
                refreshText((int)cursorIndex + 1, true);
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
        return maxCharacter != -1 && (int)allText.size() >= maxCharacter;
    }

    void Textarea::refreshText(int newCursorIndex, bool allTextUpdated) {
        //refresh cursor index
        if (    (newCursorIndex > (int)cursorIndex && cursorIndex < allText.length()) ||
                (newCursorIndex < (int)cursorIndex && cursorIndex != 0)) {
            cursorIndex = (unsigned int)newCursorIndex;
        }

        //re-compute cursor position
        computeCursorPosition();

        //refresh text
        text->updateText(std::string(stringConvert.to_bytes(allText)));

        //event
        if (allTextUpdated) {
            for (auto& eventListener : getEventListeners()) {
                eventListener->onValueChange(this);
            }
        }
    }

    void Textarea::computeCursorPosition() {
        const auto& font = text->getFont();
        cursorPosition.X = 0.0f;
        cursorPosition.Y = 0.0f; //TODO use TEXT_SHIFT_Y_PIXEL ?

        for (unsigned int i = 0; i < cursorIndex; ++i) {
            char32_t textLetter = allText[i];
            if (textLetter == '\n') {
                cursorPosition.Y += (float)font.getSpaceBetweenLines();
                cursorPosition.X = 0.0f;
            } else {
                cursorPosition.X += (float)(font.getGlyph(textLetter).width + font.getSpaceBetweenLetters());
            }
        }

        if (cursorPosition.X > 0) {
            cursorPosition.X -= (float)font.getSpaceBetweenLetters(); //remove last space
            cursorPosition.X += LETTER_AND_CURSOR_SHIFT;
        }

        cursorPosition.X += (float)widgetOutline.leftWidth;
        cursorPosition.Y += (float)widgetOutline.topWidth;
    }

    void Textarea::computeCursorIndex(int approximatePositionX, int approximatePositionY) { //TODO review for line + review for TEXT_SHIFT_Y_PIXEL
        const auto& font = text->getFont();
        auto heightText = (float)TEXT_SHIFT_Y_PIXEL + (float)font.getSpaceBetweenLines() / 2.0f;
        float widthText = 0.0f;

        for (cursorIndex = 0; cursorIndex < allText.length(); ++cursorIndex) {
            char32_t textLetter = allText[cursorIndex];
            if (textLetter == '\n') {
                heightText += (float)font.getSpaceBetweenLines();
                widthText = 0.0f;
            } else {
                widthText += (float) font.getGlyph(textLetter).width / 2.0f;
            }

            bool isEndOfLine = cursorIndex + 1 >= allText.length() || allText[cursorIndex + 1] == '\n';
            if (heightText > (float)approximatePositionY && (widthText > (float)approximatePositionX || isEndOfLine)) {
                break;
            }

            if (textLetter != '\n') {
                widthText += (float) font.getGlyph(textLetter).width / 2.0f + (float) font.getSpaceBetweenLetters();
            }
        }

        //compute the correct cursor position
        computeCursorPosition();
    }

    void Textarea::prepareWidgetRendering(float dt, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        //text box
        updateProperties(textareaRenderer.get(), projectionViewMatrix, Vector2<float>(getGlobalPositionX(), getGlobalPositionY()));
        textareaRenderer->enableRenderer(renderingOrder);

        //cursor
        cursorBlink += dt * CURSOR_BLINK_SPEED;
        if (state == ACTIVE && ((int)cursorBlink % 2) > 0) {
            renderingOrder++;
            updateProperties(cursorRenderer.get(), projectionViewMatrix, Vector2<float>(getGlobalPositionX(), getGlobalPositionY()) + cursorPosition);
            cursorRenderer->enableRenderer(renderingOrder);
        }
    }

}
