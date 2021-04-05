#include <utility>
#include "UrchinCommon.h"

#include "scene/UI/widget/textbox/TextBox.h"
#include "scene/InputDeviceKey.h"
#include "graphic/render/GenericRendererBuilder.h"

#define LETTER_SHIFT 5 //When the text box is full of text, we shift all letters to left
#define LETTER_AND_CURSOR_SHIFT 2 //Define space between the letters and cursor
#define CURSOR_BLINK_SPEED 1.75f

namespace urchin {

    TextBox::TextBox(Widget* parent, Position position, Size size, std::string nameSkin) :
            Widget(parent, position, size),
            nameSkin(std::move(nameSkin)),
            text(nullptr),
            maxWidthText(0),
            startTextIndex(0),
            cursorIndex(0),
            cursorPosition(0),
            cursorBlink(0.0f),
            state(UNACTIVE) {

    }

    TextBox::TextBox(Position position, Size size, std::string nameSkin) :
            TextBox(nullptr, position, size, std::move(nameSkin)) {

    }

    void TextBox::createOrUpdateWidget() {
        //skin information
        std::shared_ptr<XmlChunk> textBoxChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "textBox", XmlAttribute("nameSkin", nameSkin));

        std::shared_ptr<XmlChunk> skinChunkDefault = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "default"), textBoxChunk);
        texTextBoxDefault = UISkinService::instance()->createWidgetTexture(getWidth(), getHeight(), skinChunkDefault, &widgetOutline);

        std::shared_ptr<XmlChunk> skinChunkFocus = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "focus"), textBoxChunk);
        texTextBoxFocus = UISkinService::instance()->createWidgetTexture(getWidth(), getHeight(), skinChunkFocus);

        std::shared_ptr<XmlChunk> textSkinChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "textSkin", XmlAttribute(), textBoxChunk);
        delete text;
        text = new Text(this, Position(0, 0, LengthType::PIXEL), textSkinChunk->getStringValue(), "");
        text->setPosition(Position(0.0f, ((float)getHeight() - (float)text->getHeight()) / 2.0f, LengthType::PIXEL));
        maxWidthText = (unsigned int)((int)getWidth() - (widgetOutline.leftWidth + widgetOutline.rightWidth));

        Vector3<float> fontColor = text->getFont()->getFontColor();
        std::vector<unsigned char> cursorColor = {static_cast<unsigned char>(fontColor.X * 255), static_cast<unsigned char>(fontColor.Y * 255), static_cast<unsigned char>(fontColor.Z * 255), 255};
        texCursorDiffuse = Image(1, 1, Image::IMAGE_RGBA, std::move(cursorColor)).createTexture(false);
        refreshText(cursorIndex);
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
                ->addTextureReader(TextureReader::build(texTextBoxDefault, TextureParam::buildNearest()))
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
                ->addTextureReader(TextureReader::build(texCursorDiffuse, TextureParam::buildRepeatNearest()))
                ->build();
    }

    std::string TextBox::getText() {
        return allText;
    }

    bool TextBox::onKeyPressEvent(unsigned int key) {
        if (key == InputDeviceKey::MOUSE_LEFT) {
            Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX() + (int)getWidth(), getGlobalPositionY() + (int)getHeight()));
            if (widgetRectangle.collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                state = ACTIVE;
                textBoxRenderer->updateTextureReader(0, TextureReader::build(texTextBoxFocus, TextureParam::buildNearest()));

                int localMouseX = getMouseX() - text->getGlobalPositionX();
                computeCursorIndex(localMouseX);
            } else {
                state = UNACTIVE;
                textBoxRenderer->updateTextureReader(0, TextureReader::build(texTextBoxDefault, TextureParam::buildNearest()));
            }
        } else if (state == ACTIVE) {
            if (key == InputDeviceKey::LEFT_ARROW) {
                refreshText(cursorIndex - 1);
            } else if (key == InputDeviceKey::RIGHT_ARROW) {
                refreshText(cursorIndex + 1);
            } else if (key == InputDeviceKey::BACKSPACE) {
                if (cursorIndex > 0) {
                    std::string tmpRight = allText.substr((unsigned long)cursorIndex, allText.length()-cursorIndex);
                    allText = allText.substr(0, (unsigned long)(cursorIndex - 1L));
                    allText.append(tmpRight);
                    refreshText(cursorIndex - 1);
                }
            } else if (key == InputDeviceKey::DELETE_KEY) {
                if (allText.length() > 0 && cursorIndex < allText.length()) {
                    std::string tmpRight = allText.substr((unsigned long)(cursorIndex + 1L), allText.length()-cursorIndex);
                    allText = allText.substr(0, (unsigned long)cursorIndex);
                    allText.append(tmpRight);
                    refreshText(cursorIndex);
                }
            }
        }

        return true;
    }

    bool TextBox::onCharEvent(unsigned int character) {
        if (state == ACTIVE) {
            if (character < 256 && character > 30 && character != 127) {
                std::string tmpRight = allText.substr((unsigned long)cursorIndex, allText.length()-cursorIndex);
                allText = allText.substr(0, (unsigned long)cursorIndex);
                allText.append(1, static_cast<char>(character));
                allText.append(tmpRight);
                refreshText(cursorIndex + 1);
            }
            return false;
        }
        return true;
    }

    void TextBox::reset() {
        state = UNACTIVE;
        textBoxRenderer->updateTextureReader(0, TextureReader::build(texTextBoxDefault, TextureParam::buildNearest()));

        Widget::reset();
    }

    void TextBox::refreshText(unsigned int newCursorIndex) {
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
            startTextIndex = (startTextIndex > 0) ? startTextIndex-LETTER_SHIFT : 0;
        }
        computeCursorPosition();

        //determine the text to display
        const Font* font = text->getFont();
        unsigned int widthText = 0;
        unsigned int endTextIndex = startTextIndex;
        for (; endTextIndex < allText.length(); ++endTextIndex) {
            auto letter = static_cast<unsigned char>(allText[endTextIndex]);
            widthText += font->getGlyph(letter).width + font->getSpaceBetweenLetters();
            if (widthText > maxWidthText) {
                break;
            }
        }
        text->updateText(allText.substr((unsigned long)startTextIndex, (unsigned long)(endTextIndex - startTextIndex)));
    }

    void TextBox::computeCursorPosition() {
        const Font* font = text->getFont();
        cursorPosition = 0;

        for (unsigned int i = startTextIndex; i < cursorIndex; ++i) {
            auto letter = static_cast<unsigned char>(allText[i]);
            cursorPosition += font->getGlyph(letter).width + font->getSpaceBetweenLetters();
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
            auto letter = static_cast<unsigned char>(allText[cursorIndex]);
            widthText += (float)font->getGlyph(letter).width / 2.0f;
            if (widthText > (float)approximateCursorPosition) {
                break;
            }

            widthText += (float)font->getGlyph(letter).width / 2.0f + (float)font->getSpaceBetweenLetters();
        }

        //compute the correct cursor position
        computeCursorPosition();
    }

    void TextBox::prepareWidgetRendering(float dt) {
        //text box
        updateTranslateVector(textBoxRenderer, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        textBoxRenderer->addOnRenderTarget();

        //cursor
        cursorBlink += dt * CURSOR_BLINK_SPEED;
        if (state == ACTIVE && ((int)cursorBlink % 2) > 0) {
            updateTranslateVector(cursorRenderer, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()) + Vector2<int>((int)cursorPosition, 0));
            cursorRenderer->addOnRenderTarget();
        } else {
            cursorRenderer->removeFromRenderTarget();
        }
    }

}
