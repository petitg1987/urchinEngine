#include <memory>
#include <utility>
#include <codecvt>

#include <scene/ui/widget/text/Text.h>
#include <scene/ui/widget/Size.h>
#include <scene/ui/UISkinService.h>
#include <resources//MediaManager.h>
#include <graphic/render/GenericRendererBuilder.h>

namespace urchin {

    Text::Text(Widget* parent, Position position, std::string nameSkin, std::string textOrKey, bool translatable) :
            Widget(parent, position, Size(0, 0, LengthType::PIXEL)),
            nameSkin(std::move(nameSkin)),
            maxWidth(100.0f, LengthType::PERCENTAGE),
            font(nullptr) {
        if(translatable) {
            labelKey = std::move(textOrKey);
            text = "@TO_TRANSLATE@";
        } else {
            text = std::move(textOrKey);
        }

        refreshFont();
        refreshTextAndWidgetSize();
        if(parent) {
            Text::createOrUpdateWidget();
        }
    }

    Text* Text::newText(Widget* parent, Position position, std::string nameSkin, std::string text) {
        return new Text(parent, position, std::move(nameSkin), std::move(text), false);
    }

    Text* Text::newTranslatableText(Widget* parent, Position position, std::string nameSkin, std::string textKey) {
        return new Text(parent, position, std::move(nameSkin), std::move(textKey), true);
    }

    Text::~Text() {
        cleanFont();

        if(isTranslatableLabel()) {
            i18nService->remove(this);
        }
    }

    void Text::createOrUpdateWidget() {
        refreshFont();
        refreshTextAndWidgetSize();
        refreshRenderer();

        if(isTranslatableLabel()) {
            i18nService->add(this);
        }
    }

    void Text::setMaxWidth(Length maxWidth) {
        this->maxWidth = maxWidth;

        refreshTextAndWidgetSize();
        refreshRendererData();
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
        refreshRendererData();
    }

    bool Text::isTranslatableLabel() const {
        return labelKey.has_value();
    }

    const std::string& Text::getText() const {
        return text;
    }

    const std::string& Text::getLabelKey() const {
        if (!isTranslatableLabel()) {
            throw std::runtime_error("Label key requested on a no translatable label: " + text);
        }
        return labelKey.value();
    }

    void Text::updateLabel(const std::string& label) {
        updateText(label);
    }

    const Font* Text::getFont() {
        return font;
    }

    void Text::refreshTextAndWidgetSize() {
        //cut the text if needed
        cutText();

        //compute widget size
        float width = 0.0f;
        auto spaceBetweenLetters = (float)font->getSpaceBetweenLetters();
        for (auto& textLine : cutTextLines) { //each line
            float offsetX = 0.0f;
            for (char32_t textLetter : textLine) { //each letter
                auto letterWidth = (float)font->getGlyph(textLetter).width;
                offsetX += letterWidth + spaceBetweenLetters;
            }
            width = std::max(width, offsetX - spaceBetweenLetters);
        }
        std::size_t numberOfLines = cutTextLines.empty() ? 1 : cutTextLines.size();
        std::size_t numberOfInterLines = cutTextLines.empty() ? 0 : cutTextLines.size() - 1;
        auto textHeight = (float)(numberOfLines * font->getHeight() + numberOfInterLines * font->getSpaceBetweenLines());
        setSize(Size(width, textHeight, LengthType::PIXEL));
    }

    void Text::cutText() {
        cutTextLines.clear();
        std::u32string u32Text = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(text);

        std::size_t startLineIndex = 0;
        std::size_t lastSpaceIndex = 0;
        unsigned int lineLength = 0;
        unsigned int lengthFromLastSpace = 0;

        for (std::size_t letterIndex = 0; letterIndex < u32Text.length(); letterIndex++) { //each letters
            char32_t textLetter = u32Text[letterIndex];

            if (textLetter == '\n') {
                cutTextLines.emplace_back(u32Text.substr(startLineIndex, letterIndex - startLineIndex));
                startLineIndex = letterIndex + 1;
                lineLength = 0;
                lengthFromLastSpace = 0;
            } else if (textLetter == ' ') {
                lastSpaceIndex = letterIndex;
                lengthFromLastSpace = 0;
            }

            unsigned int letterLength = font->getGlyph(textLetter).width + font->getSpaceBetweenLetters();

            if (getMaxWidth() != 0 && lineLength + letterLength >= getMaxWidth()) { //cut too long line
                if((int)lastSpaceIndex - (int)startLineIndex > 0) { //cut line at last space found
                    cutTextLines.push_back(u32Text.substr(startLineIndex, lastSpaceIndex - startLineIndex));
                    startLineIndex = lastSpaceIndex + 1;
                    lineLength = lengthFromLastSpace;
                } else if((int)letterIndex - (int)startLineIndex > 0) { //cut line at the middle of a word
                    cutTextLines.push_back(u32Text.substr(startLineIndex, letterIndex - startLineIndex));
                    startLineIndex = letterIndex;
                    lineLength = letterLength;
                    lengthFromLastSpace = letterLength;
                }
            } else {
                lineLength += letterLength;
                lengthFromLastSpace += letterLength;
            }
        }

        if((int)u32Text.length() - (int)startLineIndex > 0) {
            cutTextLines.emplace_back(u32Text.substr(startLineIndex, u32Text.length() - startLineIndex));
        }
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
        Length fontHeight = UISkinService::instance()->loadLength(textChunk, "height");
        if (fontHeight.getType() == LengthType::PIXEL) {
            return (unsigned int)fontHeight.getValue();
        } else if (fontHeight.getType() == LengthType::PERCENTAGE) {
            return (unsigned int)(fontHeight.getValue() / 100.0f * (float)getSceneHeight());
        }
        throw std::runtime_error("Unknown length tpe for font height: " + std::to_string(fontHeight.getType()));
    }

    void Text::cleanFont() {
        if (font) {
            font->release();
        }
    }

    void Text::refreshCoordinates() {
        //creates the vertex array and texture array
        vertexCoord.clear();
        textureCoord.clear();
        vertexCoord.reserve(text.size() * 4);
        textureCoord.reserve(text.size() * 4);

        float offsetY = 0.0f;
        auto spaceBetweenLetters = (float)font->getSpaceBetweenLetters();
        auto spaceBetweenLines = (float)font->getSpaceBetweenLines();

        for (auto& textLine : cutTextLines) { //each line
            float offsetX = 0.0f;
            for (char32_t textLetter : textLine) { //each letter
                auto letterShift = (float)font->getGlyph(textLetter).shift;
                auto letterWidth = (float)font->getGlyph(textLetter).width;
                auto letterHeight = (float)font->getGlyph(textLetter).height;
                auto letterOffsetY = offsetY - letterShift;

                vertexCoord.emplace_back(Point2<float>(offsetX, letterOffsetY));
                vertexCoord.emplace_back(Point2<float>(letterWidth + offsetX, letterOffsetY));
                vertexCoord.emplace_back(Point2<float>(letterWidth + offsetX, letterHeight + letterOffsetY));

                vertexCoord.emplace_back(Point2<float>(offsetX, letterOffsetY));
                vertexCoord.emplace_back(Point2<float>(letterWidth + offsetX, letterHeight + letterOffsetY));
                vertexCoord.emplace_back(Point2<float>(offsetX, letterHeight + letterOffsetY));

                float sMin = (float)(textLetter % 16) / 16.0f;
                float tMin = (float)(textLetter >> 4u) / 16.0f;
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

        if (vertexCoord.empty()) {
            vertexCoord.emplace_back(Point2<float>(0.0f ,0.0f));
            vertexCoord.emplace_back(Point2<float>(0.0f ,0.0f));
            vertexCoord.emplace_back(Point2<float>(0.0f ,0.0f));
        }
        if (textureCoord.empty()) {
            textureCoord.emplace_back(Point2<float>(0.0f ,0.0f));
            textureCoord.emplace_back(Point2<float>(0.0f ,0.0f));
            textureCoord.emplace_back(Point2<float>(0.0f ,0.0f));
        }
    }

    void Text::refreshRenderer() {
        refreshCoordinates();

        textRenderer = setupUiRenderer("text", ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(font->getTexture(), TextureParam::buildLinear())) //binding 2
                ->enableTransparency()
                ->build();
    }

    void Text::refreshRendererData() {
        if(textRenderer != nullptr) {
            refreshCoordinates();

            textRenderer->updateData(0, vertexCoord);
            textRenderer->updateData(1, textureCoord);
        }
    }

    void Text::prepareWidgetRendering(float) {
        updateTranslateVector(textRenderer, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        textRenderer->enableRenderer();
    }

}
