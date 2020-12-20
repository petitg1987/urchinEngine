#include <memory>

#include "scene/GUI/widget/text/Text.h"
#include "scene/GUI/widget/Size.h"
#include "resources/MediaManager.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    Text::Text(Position position, const std::string& fontFilename) :
            Widget(position, Size(0, 0, Size::PIXEL)),
            maxLength(-1),
            font(MediaManager::instance()->getMedia<Font>(fontFilename)) {
        setText(text);
    }

    Text::~Text() {
        font->release();
    }

    void Text::createOrUpdateWidget() {
        //nothing to do: text cannot be defined in percentage (Size::SizeType::PERCENTAGE).
    }

    void Text::setText(const std::string& text, int maxLength) {
        this->text = text;
        this->maxLength = maxLength;

        //cut the text if needed
        std::size_t numLetters = 0;
        std::stringstream cutTextStream((maxLength > 0) ? cutText(text, (unsigned int)maxLength) : text);
        std::string item;
        cutTextLines.clear();
        while (std::getline(cutTextStream, item, '\n')) {
            cutTextLines.push_back(item);
            numLetters += item.size();
        }

        //creates the vertex array and texture array
        vertexCoord.clear();
        textureCoord.clear();
        vertexCoord.reserve(numLetters * 4);
        textureCoord.reserve(numLetters * 4);

        float width = 0.0f;
        float offsetY = 0.0f;
        auto spaceBetweenLetters = (float)font->getSpaceBetweenLetters();
        auto spaceBetweenLines = (float)font->getSpaceBetweenLines();

        for (auto& cutTextLine : cutTextLines) { //each lines
            float offsetX = 0.0f;
            for (char charLetter : cutTextLine) { //each letters
                auto letter = static_cast<unsigned char>(charLetter);
                auto letterShift = (float)font->getGlyph(letter).shift;
                auto letterWidth = (float)font->getGlyph(letter).width;
                auto letterHeight = (float)font->getGlyph(letter).height;
                auto letterOffsetY = offsetY - letterShift;

                vertexCoord.emplace_back(Point2<float>(offsetX, letterOffsetY));
                vertexCoord.emplace_back(Point2<float>(letterWidth + offsetX, letterOffsetY));
                vertexCoord.emplace_back(Point2<float>(letterWidth + offsetX, letterHeight + letterOffsetY));

                vertexCoord.emplace_back(Point2<float>(offsetX, letterOffsetY));
                vertexCoord.emplace_back(Point2<float>(letterWidth + offsetX, letterHeight + letterOffsetY));
                vertexCoord.emplace_back(Point2<float>(offsetX, letterHeight + letterOffsetY));

                float sMin = (float)(letter % 16) / 16.0f;
                float tMin = (float)(letter >> 4u) / 16.0f;
                float sMax = sMin + (letterWidth / (float)font->getDimensionTexture());
                float tMax = tMin + (letterHeight / (float)font->getDimensionTexture());

                textureCoord.emplace_back(Point2<float>(sMin, tMin));
                textureCoord.emplace_back(Point2<float>(sMax, tMin));
                textureCoord.emplace_back(Point2<float>(sMax, tMax));

                textureCoord.emplace_back(Point2<float>(sMin, tMin));
                textureCoord.emplace_back(Point2<float>(sMax, tMax));
                textureCoord.emplace_back(Point2<float>(sMin, tMax));

                offsetX += letterWidth + spaceBetweenLetters;

                width = std::max(width, offsetX - spaceBetweenLetters);
            }
            offsetY += spaceBetweenLines;
        }

        if (cutTextLines.empty()) { //add fake line to compute height
            cutTextLines.emplace_back("");
        }
        std::size_t numberOfInterLines = cutTextLines.size() - 1;
        setSize(Size(width, (float)(cutTextLines.size() * font->getHeight() + numberOfInterLines * font->getSpaceBetweenLines()), Size::SizeType::PIXEL));

        textRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::TRIANGLE)
                ->addData(&vertexCoord)
                ->addData(&textureCoord)
                ->addTexture(TextureReader::build(font->getTexture(), TextureParam::buildNearest()))
                ->enableTransparency()
                ->build();
    }

    std::string Text::cutText(const std::string& constText, unsigned int maxLength) {
        std::string text(constText);

        unsigned int lineLength = 0;
        unsigned int indexLastSpace = 0;
        unsigned int lengthFromLastSpace = 0;

        for (unsigned int numLetter = 0; text[numLetter] != 0; numLetter++) { //each letters
            auto letter = static_cast<unsigned char>(text[numLetter]);

            if (letter == '\n') {
                lineLength = 0;
                lengthFromLastSpace = 0;
            } else if (letter == ' ') {
                indexLastSpace = numLetter;
                lengthFromLastSpace = 0;
            }

            unsigned int lengthLetter = font->getGlyph(letter).width + font->getSpaceBetweenLetters();
            if (lineLength + lengthLetter >= maxLength) { //cut line
                text[indexLastSpace] = '\n';
                lineLength = lengthFromLastSpace;
            } else {
                lineLength += lengthLetter;
                lengthFromLastSpace += lengthLetter;
            }
        }

        return text;
    }

    const std::string& Text::getText() const {
        return text;
    }

    const Font* Text::getFont() {
        return font;
    }

    void Text::display(const RenderTarget* renderTarget, const ShaderVar& translateDistanceShaderVar, float dt) {
        if (textRenderer) {
            renderTarget->display(textRenderer);
        }

        Widget::display(renderTarget, translateDistanceShaderVar, dt);
    }

}
