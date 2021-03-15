#include <memory>
#include <utility>

#include "scene/UI/widget/text/Text.h"
#include "scene/UI/widget/Size.h"
#include "scene/UI/UISkinService.h"
#include "resources//MediaManager.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    Text::Text(Widget* parent, Position position, std::string nameSkin, std::string text) :
            Widget(parent, position, Size(0, 0, LengthType::PIXEL)),
            nameSkin(std::move(nameSkin)),
            text(std::move(text)),
            maxWidth(100.0f, LengthType::PERCENTAGE),
            font(nullptr) {
        refreshFont();
        refreshTextAndWidgetSize();
    }

    Text::Text(Position position, std::string nameSkin, std::string text) :
            Text(nullptr, position, std::move(nameSkin), std::move(text)) {

    }

    Text::~Text() {
        cleanFont();
    }

    void Text::createOrUpdateWidget() {
        refreshFont();
        refreshTextAndWidgetSize();
        refreshRenderer();
    }

    void Text::setMaxWidth(Length maxWidth) {
        this->maxWidth = maxWidth;

        refreshTextAndWidgetSize();
        refreshRenderer();
    }

    unsigned int Text::getMaxWidth() {
        if (maxWidth.getType() == LengthType::PERCENTAGE) {
            return (unsigned int)(maxWidth.getValue() / 100.0f * (float)getSceneWidth());
        }
        return (unsigned int)maxWidth.getValue();
    }

    void Text::updateText(const std::string& text) {
        this->text = text;

        refreshTextAndWidgetSize();
        refreshRenderer();
    }

    const std::string& Text::getText() const {
        return text;
    }

    const Font* Text::getFont() {
        return font;
    }

    void Text::refreshTextAndWidgetSize() {
        //cut the text if needed
        std::size_t numLetters = 0;
        std::stringstream cutTextStream(cutText(text));
        std::string item;
        cutTextLines.clear();
        while (std::getline(cutTextStream, item, '\n')) {
            cutTextLines.push_back(item);
            numLetters += item.size();
        }

        float width = 0.0f;
        auto spaceBetweenLetters = (float)font->getSpaceBetweenLetters();

        for (auto& cutTextLine : cutTextLines) { //each lines
            float offsetX = 0.0f;
            for (char charLetter : cutTextLine) { //each letters
                auto letter = static_cast<unsigned char>(charLetter);
                auto letterWidth = (float)font->getGlyph(letter).width;

                offsetX += letterWidth + spaceBetweenLetters;
            }
            width = std::max(width, offsetX - spaceBetweenLetters);
        }

        std::size_t numberOfLines = cutTextLines.empty() ? 1 : cutTextLines.size();
        std::size_t numberOfInterLines = cutTextLines.empty() ? 0 : cutTextLines.size() - 1;
        auto textHeight = (float)(numberOfLines * font->getHeight() + numberOfInterLines * font->getSpaceBetweenLines());
        setSize(Size(width, textHeight, LengthType::PIXEL));
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

    void Text::refreshFont() {
        std::shared_ptr<XmlChunk> textChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "text", XmlAttribute("nameSkin", nameSkin));
        std::string ttfFilename = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "font", XmlAttribute(), textChunk)->getStringValue();
        std::string fontColor = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "color", XmlAttribute(), textChunk)->getStringValue();
        unsigned int fontHeight = retrieveFontHeight(textChunk);

        cleanFont();
        std::map<std::string, std::string> fontParams = {{"fontSize", std::to_string(fontHeight)}, {"fontColor", fontColor}};
        font = MediaManager::instance()->getMedia<Font>(ttfFilename, fontParams);
    }

    unsigned int Text::retrieveFontHeight(const std::shared_ptr<XmlChunk>& textChunk) const {
        std::shared_ptr<XmlChunk> fontHeightChunk = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "height", XmlAttribute(), textChunk);
        float fontHeight = UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "value", XmlAttribute(), fontHeightChunk)->getFloatValue();
        LengthType fontHeightType = toLengthType(UISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "type", XmlAttribute(), fontHeightChunk)->getStringValue());

        if (fontHeightType == LengthType::PIXEL) {
            return (unsigned int)fontHeight;
        } else if (fontHeightType == LengthType::PERCENTAGE) {
            return (unsigned int)(fontHeight / 100.0f * (float)getSceneHeight());
        }
        throw std::runtime_error("Unknown font height type: " + std::to_string(fontHeightType));
    }

    LengthType Text::toLengthType(const std::string& lengthTypeString) {
        if (StringUtil::insensitiveEquals(lengthTypeString, "pixel")) {
            return LengthType::PIXEL;
        } else if (StringUtil::insensitiveEquals(lengthTypeString, "percentage")) {
            return LengthType::PERCENTAGE;
        }
        throw std::runtime_error("Unknown length type: " + lengthTypeString);
    }

    void Text::cleanFont() {
        if (font) {
            font->release();
        }
    }

    void Text::refreshRenderer() {
        //creates the vertex array and texture array
        vertexCoord.clear();
        textureCoord.clear();
        vertexCoord.reserve(text.size() * 4);
        textureCoord.reserve(text.size() * 4);

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
            }
            offsetY += spaceBetweenLines;
        }

        textRenderer = std::make_unique<GenericRendererBuilder>(getRenderTarget(), ShapeType::TRIANGLE)
                ->addData(&vertexCoord)
                ->addData(&textureCoord)
                ->addTextureReader(TextureReader::build(font->getTexture(), TextureParam::buildNearest()))
                ->enableTransparency()
                ->build();
    }

    void Text::displayWidget(const ShaderVar&, float) {
        if (textRenderer) {
            getRenderTarget()->display(textRenderer);
        }
    }

}
