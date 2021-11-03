#include <utility>
#include <codecvt>
#include <UrchinCommon.h>

#include <scene/ui/widget/textbox/TextBox.h>
#include <scene/InputDeviceKey.h>
#include <api/render/GenericRendererBuilder.h>

namespace urchin {

    TextBox::TextBox(Position position, Size size, std::string skinName) :
            Widget(position, size),
            skinName(std::move(skinName)),
            maxLength(-1),
            text(nullptr),
            maxWidthText(0),
            startTextIndex(0),
            cursorIndex(0),
            cursorPosition(0),
            cursorBlink(0.0f),
            state(INACTIVE) {

    }

    std::shared_ptr<TextBox> TextBox::create(Widget* parent, Position position, Size size, std::string skinName) {
        return Widget::create<TextBox>(new TextBox(position, size, std::move(skinName)), parent);
    }

    void TextBox::createOrUpdateWidget() {
        //detach children
        detachChild(text.get());

        //skin information
        auto textBoxChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "textBox", UdaAttribute("skin", skinName));

        auto skinChunkDefault = UISkinService::instance().getSkinReader().getUniqueChunk(true, "skin", UdaAttribute("type", "default"), textBoxChunk);
        texTextBoxDefault = UISkinService::instance().createWidgetTexture(getWidth(), getHeight(), skinChunkDefault, &widgetOutline);

        auto skinChunkFocus = UISkinService::instance().getSkinReader().getUniqueChunk(true, "skin", UdaAttribute("type", "focus"), textBoxChunk);
        texTextBoxFocus = UISkinService::instance().createWidgetTexture(getWidth(), getHeight(), skinChunkFocus);

        auto textSkinChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "textSkin", UdaAttribute(), textBoxChunk);
        text = Text::create(this, Position(0.0f, 0.0f, LengthType::PIXEL), textSkinChunk->getStringValue(), "");
        text->updatePosition(Position(0.0f, ((float)getHeight() - (float)text->getHeight()) / 2.0f, LengthType::PIXEL));
        maxWidthText = (unsigned int)((int)getWidth() - (widgetOutline.leftWidth + widgetOutline.rightWidth));

        Vector3<float> fontColor = text->getFont().getFontColor();
        std::vector<unsigned char> cursorColor = {static_cast<unsigned char>(fontColor.X * 255), static_cast<unsigned char>(fontColor.Y * 255), static_cast<unsigned char>(fontColor.Z * 255), 255};
        texCursorDiffuse = Image(1, 1, Image::IMAGE_RGBA, std::move(cursorColor), false).createTexture(false);
        refreshText((int)cursorIndex, false);
        computeCursorPosition();

        //visual
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>((float)getWidth(), 0.0f), Point2<float>((float)getWidth(), (float)getHeight()),
                Point2<float>(0.0f, 0.0f), Point2<float>((float)getWidth(), (float)getHeight()), Point2<float>(0.0f, (float)getHeight())
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        textBoxRenderer = setupUiRenderer("text box", ShapeType::TRIANGLE, false)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(texTextBoxDefault, TextureParam::buildLinear())) //binding 3
                ->build();

        auto cursorStartY = (float)widgetOutline.topWidth;
        auto cursorEndY = (float)((int)getHeight() - widgetOutline.bottomWidth);
        std::vector<Point2<float>> cursorVertexCoord = {
                Point2<float>(0.0f, cursorStartY), Point2<float>(CURSOR_WIDTH_PIXEL, cursorStartY), Point2<float>(CURSOR_WIDTH_PIXEL, cursorEndY),
                Point2<float>(0.0f, cursorStartY), Point2<float>(CURSOR_WIDTH_PIXEL, cursorEndY), Point2<float>(0.0f, cursorEndY)
        };
        std::vector<Point2<float>> cursorTextureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        cursorRenderer = setupUiRenderer("text box - cursor", ShapeType::TRIANGLE, false)
                ->addData(cursorVertexCoord)
                ->addData(cursorTextureCoord)
                ->addUniformTextureReader(TextureReader::build(texCursorDiffuse, TextureParam::buildRepeatNearest())) //binding 2
                ->build();
    }

    std::string TextBox::getText() {
        return std::string(stringConvert.to_bytes(allText));
    }

    void TextBox::setAllowedCharacters(const std::string& allowedCharacters) {
        this->allowedCharacters = stringConvert.from_bytes(allowedCharacters.c_str());
    }

    void TextBox::setMaxLength(unsigned int maxLength) {
        this->maxLength = (int)maxLength;
    }

    bool TextBox::onKeyPressEvent(unsigned int key) {
        if (key == (int)InputDeviceKey::MOUSE_LEFT) {
            if (widgetRectangle().collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                state = ACTIVE;
                textBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texTextBoxFocus, TextureParam::buildLinear()));

                int localMouseX = getMouseX() - text->getGlobalPositionX();
                computeCursorIndex(localMouseX);
            } else {
                state = INACTIVE;
                textBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texTextBoxDefault, TextureParam::buildLinear()));
            }
        } else if (state == ACTIVE) {
            if (key == (int)InputDeviceKey::LEFT_ARROW) {
                refreshText((int)cursorIndex - 1, false);
            } else if (key == (int)InputDeviceKey::RIGHT_ARROW) {
                refreshText((int)cursorIndex + 1, false);
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
            }
        }

        return true;
    }

    bool TextBox::onCharEvent(char32_t unicodeCharacter) {
        if (state == ACTIVE) {
            if (isCharacterAllowed(unicodeCharacter) && !isMaxLengthReach()) {
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

    void TextBox::onResetStateEvent() {
        state = INACTIVE;
        textBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texTextBoxDefault, TextureParam::buildLinear()));
    }

    bool TextBox::isCharacterAllowed(char32_t unicodeCharacter) const {
        return allowedCharacters.empty() || std::find(allowedCharacters.begin(), allowedCharacters.end(), unicodeCharacter) != allowedCharacters.end();
    }

    bool TextBox::isMaxLengthReach() const {
        return maxLength != -1 && (int)allText.size() >= maxLength;
    }

    void TextBox::refreshText(int newCursorIndex, bool allTextUpdated) {
        //refresh cursor index
        if (    (newCursorIndex > (int)cursorIndex && cursorIndex < allText.length()) ||
                (newCursorIndex < (int)cursorIndex && cursorIndex != 0)) {
            cursorIndex = (unsigned int)newCursorIndex;
        }

        //refresh start index
        computeCursorPosition();
        if (cursorPosition > maxWidthText) {
            startTextIndex = (startTextIndex <= allText.length()) ? startTextIndex + LETTER_SHIFT : (unsigned int)allText.length();
        } else if (cursorIndex <= startTextIndex) {
            startTextIndex = (startTextIndex > 0) ? startTextIndex - LETTER_SHIFT : 0;
        }
        computeCursorPosition();

        //determine the text to display
        const auto& font = text->getFont();
        unsigned int widthText = 0;
        unsigned int endTextIndex = startTextIndex;
        for (; endTextIndex < allText.length(); ++endTextIndex) {
            char32_t textLetter = allText[endTextIndex];
            widthText += font.getGlyph(textLetter).width + font.getSpaceBetweenLetters();
            if (widthText > maxWidthText) {
                break;
            }
        }
        U32StringA textToDisplay = allText.substr((unsigned long)startTextIndex, (unsigned long)(endTextIndex - startTextIndex));
        text->updateText(std::string(stringConvert.to_bytes(textToDisplay)));

        //event
        if (allTextUpdated) {
            for (auto& eventListener : getEventListeners()) {
                eventListener->onValueChange(this);
            }
        }
    }

    void TextBox::computeCursorPosition() {
        const auto& font = text->getFont();
        cursorPosition = 0;

        for (unsigned int i = startTextIndex; i < cursorIndex; ++i) {
            char32_t textLetter = allText[i];
            cursorPosition += font.getGlyph(textLetter).width + font.getSpaceBetweenLetters();
        }

        if (cursorPosition > 0) {
            cursorPosition -= font.getSpaceBetweenLetters(); //remove last space
            cursorPosition += LETTER_AND_CURSOR_SHIFT;
        }

        cursorPosition += (unsigned int)widgetOutline.leftWidth;
    }

    void TextBox::computeCursorIndex(int approximateCursorPosition) {
        const auto& font = text->getFont();
        float widthText = 0.0f;

        for (cursorIndex = startTextIndex; cursorIndex < allText.length(); ++cursorIndex) {
            char32_t textLetter = allText[cursorIndex];
            widthText += (float)font.getGlyph(textLetter).width / 2.0f;
            if (widthText > (float)approximateCursorPosition) {
                break;
            }

            widthText += (float)font.getGlyph(textLetter).width / 2.0f + (float)font.getSpaceBetweenLetters();
        }

        //compute the correct cursor position
        computeCursorPosition();
    }

    void TextBox::prepareWidgetRendering(float dt, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        //text box
        updatePositioning(textBoxRenderer.get(), projectionViewMatrix, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        textBoxRenderer->enableRenderer(renderingOrder);

        //cursor
        cursorBlink += dt * CURSOR_BLINK_SPEED;
        if (state == ACTIVE && ((int)cursorBlink % 2) > 0) {
            renderingOrder++;
            updatePositioning(cursorRenderer.get(), projectionViewMatrix, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()) + Vector2<int>((int)cursorPosition, 0));
            cursorRenderer->enableRenderer(renderingOrder);
        }
    }

}
