#include <utility>
#include <codecvt>
#include <UrchinCommon.h>

#include <scene/ui/widget/textbox/TextBox.h>
#include <scene/ui/widget/InputTextHelper.h>
#include <scene/InputDeviceKey.h>
#include <api/render/GenericRendererBuilder.h>

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
            state(INACTIVE) {

    }

    std::shared_ptr<TextBox> TextBox::create(Widget* parent, Position position, Size size, std::string skinName) {
        return Widget::create<TextBox>(new TextBox(position, size, std::move(skinName)), parent);
    }

    void TextBox::createOrUpdateWidget() {
        //detach children
        detachChild(text.get());

        //skin information
        auto textBoxChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textBox", UdaAttribute("skin", skinName));

        auto skinChunkDefault = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "default"), textBoxChunk);
        texTextBoxDefault = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunkDefault, &widgetOutline);

        auto skinChunkFocus = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute("type", "focus"), textBoxChunk);
        texTextBoxFocus = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunkFocus);

        auto textSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textSkin", UdaAttribute(), textBoxChunk);
        text = Text::create(this, Position(0.0f, 0.0f, LengthType::PIXEL), textSkinChunk->getStringValue(), "");
        maxWidthText = (unsigned int)((int)getWidth() - (widgetOutline.leftWidth + widgetOutline.rightWidth));

        Vector3<float> fontColor = text->getFont().getFontColor();
        std::vector<unsigned char> cursorColor = {static_cast<unsigned char>(fontColor.X * 255), static_cast<unsigned char>(fontColor.Y * 255), static_cast<unsigned char>(fontColor.Z * 255), 255};
        texCursorAlbedo = Texture::build("cursor albedo", 1, 1, TextureFormat::RGBA_8_INT, cursorColor.data());
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
        textBoxRenderer = setupUiRenderer("text box", ShapeType::TRIANGLE, false)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(texTextBoxDefault, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy()))) //binding 3
                ->build();

        auto cursorStartY = (float)widgetOutline.topWidth - (float)InputTextHelper::CURSOR_HEIGHT_MARGIN_PIXEL;
        auto cursorEndY = (float)cursorStartY + (float)text->getFont().getHeight() + ((float)InputTextHelper::CURSOR_HEIGHT_MARGIN_PIXEL * 2.0f);
        std::vector<Point2<float>> cursorVertexCoord = {
                Point2<float>(0.0f, cursorStartY), Point2<float>(InputTextHelper::CURSOR_WIDTH_PIXEL, cursorStartY), Point2<float>(InputTextHelper::CURSOR_WIDTH_PIXEL, cursorEndY),
                Point2<float>(0.0f, cursorStartY), Point2<float>(InputTextHelper::CURSOR_WIDTH_PIXEL, cursorEndY), Point2<float>(0.0f, cursorEndY)
        };
        std::vector<Point2<float>> cursorTextureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        cursorRenderer = setupUiRenderer("text box - cursor", ShapeType::TRIANGLE, false)
                ->addData(cursorVertexCoord)
                ->addData(cursorTextureCoord)
                ->addUniformTextureReader(TextureReader::build(texCursorAlbedo, TextureParam::build(TextureParam::REPEAT, TextureParam::NEAREST, getTextureAnisotropy()))) //binding 3
                ->build();
    }

    WidgetType TextBox::getWidgetType() const {
        return WidgetType::TEXTBOX;
    }

    std::string TextBox::getText() {
        return std::string(stringConvert.to_bytes(originalText));
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
                computeCursorIndex(localMouseX, localMouseY);
            } else {
                state = INACTIVE;
                textBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texTextBoxDefault, TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy())));
            }
        } else if (state == ACTIVE) {
            if (key == (int)InputDeviceKey::LEFT_ARROW) {
                if (cursorIndex > 0) {
                    cursorIndex--;
                    refreshText(false);
                }
            } else if (key == (int)InputDeviceKey::RIGHT_ARROW) {
                assert(cursorIndex <= originalText.length());
                if (cursorIndex < originalText.length()) {
                    cursorIndex++;
                    refreshText(false);
                }
            } else if (key == (int)InputDeviceKey::BACKSPACE) {
                if (cursorIndex > 0) {
                    U32StringA tmpRight = originalText.substr((unsigned long)cursorIndex, originalText.length() - cursorIndex);
                    originalText = originalText.substr(0, (unsigned long)(cursorIndex - 1L));
                    originalText.append(tmpRight);

                    cursorIndex--;
                    refreshText(true);
                }
            } else if (key == (int)InputDeviceKey::DELETE_KEY) {
                if (cursorIndex < originalText.length()) {
                    U32StringA tmpRight = originalText.substr((unsigned long)(cursorIndex + 1L), originalText.length() - cursorIndex);
                    originalText = originalText.substr(0, (unsigned long)cursorIndex);
                    originalText.append(tmpRight);

                    refreshText(true);
                }
            }
        }
        return true;
    }

    bool TextBox::onCharEvent(char32_t unicodeCharacter) {
        if (state == ACTIVE) {
            if (isCharacterAllowed(unicodeCharacter) && !isMaxCharacterReach()) {
                U32StringA tmpRight = originalText.substr((unsigned long)cursorIndex, originalText.length() - cursorIndex);
                originalText = originalText.substr(0, (unsigned long)cursorIndex);
                originalText.append(1, unicodeCharacter);
                originalText.append(tmpRight);

                cursorIndex++;
                refreshText(true);
            }
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
        refreshCursorPosition();
        if (cursorPosition.X > (int)maxWidthText) {
            startTextIndex = (startTextIndex <= originalText.length()) ? startTextIndex + LETTER_SHIFT : (unsigned int)originalText.length();
        } else if (cursorIndex <= startTextIndex) {
            startTextIndex = (startTextIndex > 0) ? startTextIndex - LETTER_SHIFT : 0;
        }
        refreshCursorPosition();

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

    void TextBox::refreshCursorPosition() {
        cursorPosition.X = 0.0f;
        cursorPosition.Y = 0.0f;

        for (unsigned int i = startTextIndex; i < cursorIndex; ++i) {
            char32_t textLetter = originalText[i];
            cursorPosition.X += (int)(text->getFont().getGlyph(textLetter).width + text->getFont().getSpaceBetweenLetters());
        }

        if (cursorPosition.X > 0) {
            cursorPosition.X -= (int)text->getFont().getSpaceBetweenLetters(); //remove last space
            cursorPosition.X += InputTextHelper::LETTER_AND_CURSOR_SHIFT;
        }

        cursorBlink = 0.0f;
    }

    void TextBox::computeCursorIndex(int approximatePositionX, int) {
        const auto& font = text->getFont();
        float widthText = 0.0f;

        for (cursorIndex = startTextIndex; cursorIndex < originalText.length(); ++cursorIndex) {
            char32_t textLetter = originalText[cursorIndex];
            widthText += (float)font.getGlyph(textLetter).width / 2.0f;
            if (widthText > (float)approximatePositionX) {
                break;
            }

            widthText += (float)font.getGlyph(textLetter).width / 2.0f + (float)font.getSpaceBetweenLetters();
        }

        //compute the correct cursor position
        refreshCursorPosition();
    }

    void TextBox::prepareWidgetRendering(float dt, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        //text box
        updateProperties(textBoxRenderer.get(), projectionViewMatrix, Vector2<float>(getGlobalPositionX(), getGlobalPositionY()));
        textBoxRenderer->enableRenderer(renderingOrder);

        //cursor
        cursorBlink += dt * InputTextHelper::CURSOR_BLINK_SPEED;
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
