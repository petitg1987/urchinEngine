#include <utility>
#include <codecvt>
#include <UrchinCommon.h>

#include <scene/ui/widget/textbox/TextBox.h>
#include <scene/InputDeviceKey.h>
#include <graphic/render/GenericRendererBuilder.h>

namespace urchin {

    //static
    constexpr unsigned int TextBox::LETTER_SHIFT = 5; //when the text box is full of text, we shift all letters to left
    constexpr unsigned int TextBox::LETTER_AND_CURSOR_SHIFT = 2; //define space between the letters and cursor
    constexpr float TextBox::CURSOR_BLINK_SPEED = 1.75f;

    TextBox::TextBox(Widget* parent, Position position, Size size, std::string nameSkin) :
            Widget(parent, position, size),
            nameSkin(std::move(nameSkin)),
            text(nullptr),
            maxWidthText(0),
            startTextIndex(0),
            cursorIndex(0),
            cursorPosition(0),
            cursorBlink(0.0f),
            state(INACTIVE) {
        if(parent) {
            TextBox::createOrUpdateWidget();
        }
    }

    void TextBox::createOrUpdateWidget() {
        //skin information
        auto textBoxChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "textBox", XmlAttribute("nameSkin", nameSkin));

        auto skinChunkDefault = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "skin", XmlAttribute("type", "default"), textBoxChunk.get());
        texTextBoxDefault = UISkinService::instance()->createWidgetTexture(getWidth(), getHeight(), skinChunkDefault.get(), &widgetOutline);

        auto skinChunkFocus = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "skin", XmlAttribute("type", "focus"), textBoxChunk.get());
        texTextBoxFocus = UISkinService::instance()->createWidgetTexture(getWidth(), getHeight(), skinChunkFocus.get());

        auto textSkinChunk = UISkinService::instance()->getSkinReader()->getUniqueChunk(true, "textSkin", XmlAttribute(), textBoxChunk.get());
        delete text;
        text = Text::newText(this, Position(0, 0, LengthType::PIXEL), textSkinChunk->getStringValue(), "");
        text->updatePosition(Position(0.0f, ((float)getHeight() - (float)text->getHeight()) / 2.0f, LengthType::PIXEL));
        maxWidthText = (unsigned int)((int)getWidth() - (widgetOutline.leftWidth + widgetOutline.rightWidth));

        Vector3<float> fontColor = text->getFont()->getFontColor();
        std::vector<unsigned char> cursorColor = {static_cast<unsigned char>(fontColor.X * 255), static_cast<unsigned char>(fontColor.Y * 255), static_cast<unsigned char>(fontColor.Z * 255), 255};
        texCursorDiffuse = Image(1, 1, Image::IMAGE_RGBA, std::move(cursorColor)).createTexture(false);
        refreshText(cursorIndex, false);
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
        textBoxRenderer = setupUiRenderer("text box", ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(texTextBoxDefault, TextureParam::buildLinear())) //binding 3
                ->build();

        std::vector<Point2<float>> cursorVertexCoord = {
                Point2<float>(0.0f, (float)widgetOutline.topWidth),
                Point2<float>(0.0f, (float)((int)getHeight() - widgetOutline.bottomWidth))
        };
        std::vector<Point2<float>> cursorTextureCoord = {
                Point2<float>(0.0, 0.0),
                Point2<float>(1.0, 1.0)
        };
        cursorRenderer = setupUiRenderer("text box - cursor", ShapeType::LINE)
                ->addData(cursorVertexCoord)
                ->addData(cursorTextureCoord)
                ->addUniformTextureReader(TextureReader::build(texCursorDiffuse, TextureParam::buildRepeatNearest())) //binding 2
                ->build();
    }

    std::string TextBox::getText() {
        return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.to_bytes(allText);
    }

    bool TextBox::onKeyPressEvent(unsigned int key) {
        if (key == InputDeviceKey::MOUSE_LEFT) {
            Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX() + (int)getWidth(), getGlobalPositionY() + (int)getHeight()));
            if (widgetRectangle.collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                state = ACTIVE;
                textBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texTextBoxFocus, TextureParam::buildLinear()));

                int localMouseX = getMouseX() - text->getGlobalPositionX();
                computeCursorIndex(localMouseX);
            } else {
                state = INACTIVE;
                textBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texTextBoxDefault, TextureParam::buildLinear()));
            }
        } else if (state == ACTIVE) {
            if (key == InputDeviceKey::LEFT_ARROW) {
                refreshText(cursorIndex - 1, false);
            } else if (key == InputDeviceKey::RIGHT_ARROW) {
                refreshText(cursorIndex + 1, false);
            } else if (key == InputDeviceKey::BACKSPACE) {
                if (cursorIndex > 0) {
                    std::u32string tmpRight = allText.substr((unsigned long)cursorIndex, allText.length() - cursorIndex);
                    allText = allText.substr(0, (unsigned long)(cursorIndex - 1L));
                    allText.append(tmpRight);
                    refreshText(cursorIndex - 1, true);
                }
            } else if (key == InputDeviceKey::DELETE_KEY) {
                if (allText.length() > 0 && cursorIndex < allText.length()) {
                    std::u32string tmpRight = allText.substr((unsigned long)(cursorIndex + 1L), allText.length() - cursorIndex);
                    allText = allText.substr(0, (unsigned long)cursorIndex);
                    allText.append(tmpRight);
                    refreshText(cursorIndex, true);
                }
            }
        }

        return true;
    }

    bool TextBox::onCharEvent(char32_t unicodeCharacter) {
        if (state == ACTIVE) {
            std::u32string tmpRight = allText.substr((unsigned long)cursorIndex, allText.length() - cursorIndex);
            allText = allText.substr(0, (unsigned long)cursorIndex);
            allText.append(1, unicodeCharacter);
            allText.append(tmpRight);
            refreshText(cursorIndex + 1, true);

            return false;
        }
        return true;
    }

    void TextBox::onResetState() {
        state = INACTIVE;
        textBoxRenderer->updateUniformTextureReader(0, TextureReader::build(texTextBoxDefault, TextureParam::buildLinear()));

        Widget::onResetState();
    }

    void TextBox::refreshText(unsigned int newCursorIndex, bool allTextUpdated) {
        //refresh cursor index
        if (    (newCursorIndex > cursorIndex && cursorIndex < allText.length()) ||
                (newCursorIndex < cursorIndex && cursorIndex != 0)) {
            cursorIndex = newCursorIndex;
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
        const Font* font = text->getFont();
        unsigned int widthText = 0;
        unsigned int endTextIndex = startTextIndex;
        for (; endTextIndex < allText.length(); ++endTextIndex) {
            char32_t textLetter = allText[endTextIndex];
            widthText += font->getGlyph(textLetter).width + font->getSpaceBetweenLetters();
            if (widthText > maxWidthText) {
                break;
            }
        }
        std::u32string textToDisplay = allText.substr((unsigned long)startTextIndex, (unsigned long)(endTextIndex - startTextIndex));
        text->updateText(std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.to_bytes(textToDisplay));

        //event
        if(allTextUpdated) {
            for (auto& eventListener : getEventListeners()) {
                eventListener->onValueChange(this);
            }
        }
    }

    void TextBox::computeCursorPosition() {
        const Font* font = text->getFont();
        cursorPosition = 0;

        for (unsigned int i = startTextIndex; i < cursorIndex; ++i) {
            char32_t textLetter = allText[i];
            cursorPosition += font->getGlyph(textLetter).width + font->getSpaceBetweenLetters();
        }

        if (cursorPosition > 0) {
            cursorPosition -= font->getSpaceBetweenLetters(); //remove last space
            cursorPosition += LETTER_AND_CURSOR_SHIFT;
        }

        cursorPosition += (unsigned int)widgetOutline.leftWidth;
    }

    void TextBox::computeCursorIndex(int approximateCursorPosition) {
        const Font* font = text->getFont();
        float widthText = 0.0f;

        for (cursorIndex = startTextIndex; cursorIndex < allText.length(); ++cursorIndex) {
            char32_t textLetter = allText[cursorIndex];
            widthText += (float)font->getGlyph(textLetter).width / 2.0f;
            if (widthText > (float)approximateCursorPosition) {
                break;
            }

            widthText += (float)font->getGlyph(textLetter).width / 2.0f + (float)font->getSpaceBetweenLetters();
        }

        //compute the correct cursor position
        computeCursorPosition();
    }

    void TextBox::prepareWidgetRendering(float dt) {
        //text box
        updateTranslateVector(textBoxRenderer, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        textBoxRenderer->enableRenderer();

        //cursor
        cursorBlink += dt * CURSOR_BLINK_SPEED;
        if (state == ACTIVE && ((int)cursorBlink % 2) > 0) {
            updateTranslateVector(cursorRenderer, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()) + Vector2<int>((int)cursorPosition, 0));
            cursorRenderer->enableRenderer();
        }
    }

}
