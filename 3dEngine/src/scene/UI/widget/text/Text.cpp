#include <memory>

#include "scene/UI/widget/text/Text.h"
#include "scene/UI/widget/Size.h"
#include "scene/UI/UISkinService.h"
#include "resources//MediaManager.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    Text::Text(Position position, std::string nameSkin, std::string text) :
            Widget(position, Size(0, 0, Size::PIXEL)),
            nameSkin(std::move(nameSkin)),
            fontHeight(Length(1.0f, Length::PIXEL)),
            text(std::move(text)),
            maxWidth(9999999.0 /* Cannot use numeric_limit<float>::max() because overflow during cast */, Length::PIXEL),
            font(nullptr) {
        Text::createOrUpdateWidget();
    }

    Text::~Text() {
        cleanFont();
    }

    void Text::createOrUpdateWidget() {
        std::shared_ptr<XmlChunk> textChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "text", XmlAttribute("nameSkin", nameSkin));
        std::string fontFilename = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "font", XmlAttribute(), textChunk)->getStringValue();
        fontHeight = loadFontHeight(textChunk);

        cleanFont();
        std::map<std::string, std::string> fontParams = {{"size", std::to_string(getFontHeight())}};
        font = MediaManager::instance()->getMedia<Font>(fontFilename, fontParams);

        createRender();
    }

    Length Text::loadFontHeight(const std::shared_ptr<XmlChunk>& textChunk) {
        std::shared_ptr<XmlChunk> fontHeightChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "height", XmlAttribute(), textChunk);
        float fontHeight = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "value", XmlAttribute(), fontHeightChunk)->getFloatValue();
        std::string fontHeightTypeString = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "type", XmlAttribute(), fontHeightChunk)->getStringValue();

        if(StringUtil::insensitiveEquals(fontHeightTypeString, "pixel")) {
            return Length(fontHeight, Length::PIXEL);
        }else if(StringUtil::insensitiveEquals(fontHeightTypeString, "percentage")) {
            return Length(fontHeight, Length::PERCENTAGE);
        } else {
            throw std::runtime_error("Unknown font height type: " + fontHeightTypeString);
        }
    }

    void Text::setMaxWidth(Length maxWidth) {
        this->maxWidth = maxWidth;

        createOrUpdateWidget();
    }

    void Text::updateText(const std::string& text) {
        this->text = text;

        createOrUpdateWidget();
    }

    void Text::createRender() {
        //cut the text if needed
        std::size_t numLetters = 0;
        std::stringstream cutTextStream(cutText(text));
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
        auto textHeight = (float)(cutTextLines.size() * font->getHeight() + numberOfInterLines * font->getSpaceBetweenLines());
        setSize(Size(width, textHeight, Size::SizeType::PIXEL));

        textRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::TRIANGLE)
                ->addData(&vertexCoord)
                ->addData(&textureCoord)
                ->addTexture(TextureReader::build(font->getTexture(), TextureParam::buildNearest()))
                ->enableTransparency()
                ->build();
    }

    const std::string& Text::getText() const {
        return text;
    }

    const Font* Text::getFont() {
        return font;
    }

    void Text::cleanFont() {
        if(font) {
            font->release();
        }
    }

    std::string Text::cutText(const std::string& constText) {
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
            if (lineLength + lengthLetter >= getMaxWidth()) { //cut line
                text[indexLastSpace] = '\n';
                lineLength = lengthFromLastSpace;
            } else {
                lineLength += lengthLetter;
                lengthFromLastSpace += lengthLetter;
            }
        }

        return text;
    }

    unsigned int Text::getFontHeight() {
        if (fontHeight.getType() == Length::PERCENTAGE) {
            return (unsigned int)(fontHeight.getValue() * (float)getSceneHeight());
        }
        return (unsigned int)fontHeight.getValue();
    }

    unsigned int Text::getMaxWidth() {
        if (maxWidth.getType() == Length::PERCENTAGE) {
            return (unsigned int)(maxWidth.getValue() * (float)getSceneWidth());
        }
        return (unsigned int)maxWidth.getValue();
    }

    void Text::display(const RenderTarget* renderTarget, const ShaderVar& translateDistanceShaderVar, float dt) {
        if (textRenderer) {
            renderTarget->display(textRenderer);
        }

        Widget::display(renderTarget, translateDistanceShaderVar, dt);
    }

}
