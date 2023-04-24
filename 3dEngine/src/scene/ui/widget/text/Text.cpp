#include <memory>
#include <utility>

#include <scene/ui/widget/text/Text.h>
#include <scene/ui/widget/container/Container.h>
#include <scene/ui/widget/Size.h>
#include <scene/ui/displayer/WidgetInstanceDisplayer.h>
#include <scene/ui/UISkinService.h>
#include <resources//ResourceRetriever.h>

namespace urchin {

    Text::Text(Position position, std::string skinName, std::string inputText, std::vector<std::string> inputTextParameters) :
            Widget(position, Size(0, 0, LengthType::PIXEL)),
            skinName(std::move(skinName)),
            inputText(std::move(inputText), std::move(inputTextParameters)),
            maxWidth(-1.0f),
            maxWidthType(LengthType::PIXEL),
            font(nullptr) {

    }

    std::shared_ptr<Text> Text::create(Widget* parent, Position position, std::string skinName, std::string inputText) {
        return Widget::create<Text>(new Text(position, std::move(skinName), std::move(inputText), {}), parent);
    }

    std::shared_ptr<Text> Text::create(Widget* parent, Position position, std::string skinName, const ParameterizedText& parameterizedText) {
        return Widget::create<Text>(new Text(position, std::move(skinName), parameterizedText.getText(), parameterizedText.getParameters()), parent);
    }

    Text::~Text() {
        if (isInitialized()) { //two cases of not initialized: the text has not been added to an UIRenderer or the UIRenderer has been removed before the text
            if (inputText.hasTranslatableText()) {
                getI18nService().remove(this);
            }
        }
    }

    void Text::createOrUpdateWidget() {
        refreshFont();

        if (inputText.hasTranslatableText()) {
            getI18nService().add(this);
        } else {
            baseText = inputText.translate(std::nullopt);
            refreshTextAndWidgetSize();
        }

        std::unique_ptr<WidgetInstanceDisplayer> displayer = std::make_unique<WidgetInstanceDisplayer>(getUiRenderer());
        displayer->addInstanceWidget(*this);
        setupDisplayer(std::move(displayer));
    }

    void Text::uninitialize() {
        assert(isInitialized());
        if (inputText.hasTranslatableText()) {
            getI18nService().remove(this);
        }
        Widget::uninitialize();
    }

    WidgetType Text::getWidgetType() const {
        return WidgetType::TEXT;
    }

    void Text::updateSize(Size) {
        throw std::runtime_error("Update size is not allowed on text: " + inputText.getText());
    }

    void Text::setMaxWidth(float maxWidth, LengthType maxWidthType) {
        this->maxWidth = maxWidth;
        this->maxWidthType = maxWidthType;

        if (isInitialized()) {
            refreshTextAndWidgetSize();
        }
    }

    int Text::getMaxWidthPixel() const {
        if (maxWidthType == LengthType::PIXEL) {
            return MathFunction::roundToInt(maxWidth);
        } else if (maxWidthType == LengthType::SCREEN_PERCENT) {
            return MathFunction::roundToInt(maxWidth / 100.0f * (float)getSceneSize().X);
        } else if (maxWidthType == LengthType::CONTAINER_PERCENT) {
            if (!getParentContainer()) {
                throw std::runtime_error("Missing parent container on the text widget: " + baseText);
            }
            return MathFunction::roundToInt(maxWidth / 100.0f * getParentContainer()->getWidth());
        } else if (maxWidthType == LengthType::PARENT_PERCENT) {
            if (!getParent()) {
                throw std::runtime_error("Missing parent on the text widget: " + baseText);
            }
            return MathFunction::roundToInt(maxWidth / 100.0f * getParent()->getWidth());
        }
        throw std::runtime_error("Unknown max width type: " + std::to_string(maxWidthType));
    }

    void Text::updateText(std::string inputText) {
        updateText(ParameterizedText(std::move(inputText), {}));
    }

    void Text::updateText(ParameterizedText parameterizedText) {
        if (inputText.hasTranslatableText()) {
            getI18nService().remove(this);
        }

        inputText = std::move(parameterizedText);

        if (!inputText.hasTranslatableText()) {
            baseText = inputText.translate(std::nullopt);
            refreshTextAndWidgetSize();
        } else {
            getI18nService().add(this);
        }
    }

    /**
     * @return input text translated for i18n text
     */
    const std::string& Text::getBaseText() const {
        return baseText;
    }

    /**
     * @return base text cut in line
     */
    const std::vector<TextLine>& Text::getCutTextLines() const {
        return cutTextLines;
    }

    std::size_t Text::cutTextIndexToBaseTextIndex(std::size_t cutTextIndex) const {
        int delta = 0;
        std::size_t currentIndex = 0;
        for (const TextLine& lineIndex : cutTextLines) {
            for (std::size_t columnIndex = 0; columnIndex <= lineIndex.text.length(); ++columnIndex) {
                if (currentIndex == cutTextIndex) {
                    return (std::size_t)((long)cutTextIndex + delta);
                } else {
                    currentIndex++;
                }
            }
            if (lineIndex.cutType == TextCutType::MIDDLE_WORD) {
                delta--;
            }
        }
        throw std::runtime_error("Cut text lines index " + std::to_string(cutTextIndex) + " does not exist for text: " + baseText);
    }

    std::size_t Text::baseTextIndexToCutTextIndex(std::size_t baseTextIndex, WordCutIndexPositioning wordCutIndexPositioning) const {
        std::size_t cutTextDelta = 0;
        std::size_t currentCutTextIndex = 0;
        for (const TextLine& lineIndex : cutTextLines) {
            for (std::size_t columnIndex = 0; columnIndex <= lineIndex.text.length(); ++columnIndex) {
                if (currentCutTextIndex == baseTextIndex + cutTextDelta) {
                    if (columnIndex == lineIndex.text.length() && lineIndex.cutType == TextCutType::MIDDLE_WORD && wordCutIndexPositioning == WordCutIndexPositioning::BEGIN_OF_NEXT_LINE) {
                        //index cannot be at end of line when cut type is middle of word
                        return currentCutTextIndex + 1;
                    }
                    return currentCutTextIndex;
                } else if (columnIndex == lineIndex.text.length() && lineIndex.cutType == TextCutType::MIDDLE_WORD) {
                    currentCutTextIndex++;
                    cutTextDelta++;
                } else {
                    currentCutTextIndex++;
                }
            }
        }
        throw std::runtime_error("Base text index " + std::to_string(baseTextIndex) + " does not exist for text: " + baseText);
    }

    std::size_t Text::baseTextIndexToEndOfLineIndex(std::size_t baseTextIndex, WordCutIndexPositioning wordCutIndexPositioning) const {
        std::size_t cutTextIndex = baseTextIndexToCutTextIndex(baseTextIndex, wordCutIndexPositioning);

        std::size_t currentIndex = 0;
        for (const TextLine& textLine : cutTextLines) {
            if (currentIndex + textLine.text.length() >= cutTextIndex) {
                std::size_t textEndOfCurrentLineIndex = currentIndex + textLine.text.length();
                std::size_t endOfLineDelta = textEndOfCurrentLineIndex - cutTextIndex;
                return baseTextIndex + endOfLineDelta;
            }

            currentIndex += textLine.text.length() + 1 /* for line return */;
        }
        throw std::runtime_error("Base text index " + std::to_string(baseTextIndex) + " does not exist for text: " + baseText);
    }

    void Text::refreshTranslation(const LanguageTranslator& languageTranslator) {
        baseText = inputText.translate(std::make_optional(languageTranslator));
        refreshTextAndWidgetSize();
    }

    const Font& Text::getFont() const {
        return *font;
    }

     void Text::retrieveVertexCoordinates(std::vector<Point2<float>>& vertexCoord) {
        assert(vertexCoord.empty());
        vertexCoord.reserve(baseText.size() * 4);

        float offsetY = 0.0f;
        auto spaceBetweenCharacters = (float)font->getSpaceBetweenCharacters();
        auto spaceBetweenLines = (float)font->getSpaceBetweenLines();

        for (const TextLine& textLine : cutTextLines) { //each line
            float offsetX = 0.0f;
            for (char32_t textLetter : textLine.text) { //each letter
                auto letterShift = (float)font->getGlyph(textLetter).shift;
                auto letterWidth = (float)font->getGlyph(textLetter).width;
                auto letterHeight = (float)font->getGlyph(textLetter).height;
                auto letterOffsetY = offsetY - letterShift;

                vertexCoord.emplace_back(offsetX, letterOffsetY);
                vertexCoord.emplace_back(letterWidth + offsetX, letterOffsetY);
                vertexCoord.emplace_back(letterWidth + offsetX, letterHeight + letterOffsetY);

                vertexCoord.emplace_back(offsetX, letterOffsetY);
                vertexCoord.emplace_back(letterWidth + offsetX, letterHeight + letterOffsetY);
                vertexCoord.emplace_back(offsetX, letterHeight + letterOffsetY);

                offsetX += letterWidth + spaceBetweenCharacters;
            }
            offsetY += spaceBetweenLines;
        }

        if (vertexCoord.empty()) {
            vertexCoord.emplace_back(0.0f ,0.0f);
            vertexCoord.emplace_back(0.0f ,0.0f);
            vertexCoord.emplace_back(0.0f ,0.0f);
        }
    }

     void Text::retrieveTextureCoordinates(std::vector<Point2<float>>& textureCoord) {
        assert(textureCoord.empty());
        textureCoord.reserve(baseText.size() * 4);

        for (const TextLine& textLine : cutTextLines) { //each line
            for (char32_t textLetter : textLine.text) { //each letter
                auto letterWidth = (float)font->getGlyph(textLetter).width;
                auto letterHeight = (float)font->getGlyph(textLetter).height;

                float sMin = (float)(textLetter % 16) / 16.0f;
                float tMin = (float)(textLetter >> 4u) / 16.0f;
                float sMax = sMin + (letterWidth / (float)font->getDimensionTexture());
                float tMax = tMin + (letterHeight / (float)font->getDimensionTexture());

                textureCoord.emplace_back(sMin, tMin);
                textureCoord.emplace_back(sMax, tMin);
                textureCoord.emplace_back(sMax, tMax);

                textureCoord.emplace_back(sMin, tMin);
                textureCoord.emplace_back(sMax, tMax);
                textureCoord.emplace_back(sMin, tMax);
            }
        }

        if (textureCoord.empty()) {
            textureCoord.emplace_back(0.0f ,0.0f);
            textureCoord.emplace_back(0.0f ,0.0f);
            textureCoord.emplace_back(0.0f ,0.0f);
        }
    }

    void Text::refreshTextAndWidgetSize() {
        //cut the text if needed
        cutText();

        //compute widget size
        float width = 0.0f;
        auto spaceBetweenLetters = (float)font->getSpaceBetweenCharacters();
        for (const auto& textLine : cutTextLines) { //each line
            float offsetX = 0.0f;
            for (char32_t textLetter : textLine.text) { //each letter
                auto letterWidth = (float)font->getGlyph(textLetter).width;
                offsetX += letterWidth + spaceBetweenLetters;
            }
            width = std::max(width, offsetX - spaceBetweenLetters);
        }
        std::size_t numberOfInterLines = cutTextLines.empty() ? 0 : cutTextLines.size() - 1;
        auto textHeight = (float)(font->getHeight() + (numberOfInterLines * font->getSpaceBetweenLines()));
        Widget::updateSize(Size(width, textHeight, LengthType::PIXEL));
    }

    void Text::cutText() {
        cutTextLines.clear();
        U32StringA u32Text = stringConvert.from_bytes(baseText.c_str());

        std::size_t startLineIndex = 0;

        int maxWidthPixel = getMaxWidthPixel();
        if (maxWidthPixel > 0) {
            std::size_t lastSpaceIndex = 0;
            unsigned int lineLength = 0;
            unsigned int lengthFromLastSpace = 0;

            for (std::size_t letterIndex = 0; letterIndex < u32Text.length(); letterIndex++) { //each letters
                char32_t textLetter = u32Text[letterIndex];

                if (textLetter == '\n') {
                    cutTextLines.emplace_back(TextLine{.text = u32Text.substr(startLineIndex, letterIndex - startLineIndex), .cutType = TextCutType::CLASSIC});
                    startLineIndex = letterIndex + 1;
                    lineLength = 0;
                    lengthFromLastSpace = 0;
                    continue;
                } else if (textLetter == ' ') {
                    lastSpaceIndex = letterIndex;
                    lengthFromLastSpace = 0;
                }

                unsigned int letterLength = font->getGlyph(textLetter).width + font->getSpaceBetweenCharacters();

                if (lineLength + letterLength >= (unsigned int)maxWidthPixel) { //cut too long line
                    if ((int)lastSpaceIndex - (int)startLineIndex > 0) { //cut line at last space found
                        cutTextLines.emplace_back(TextLine{.text = u32Text.substr(startLineIndex, lastSpaceIndex - startLineIndex), .cutType = TextCutType::WORD});
                        startLineIndex = lastSpaceIndex + 1;
                        lineLength = lengthFromLastSpace;
                    } else if ((int)letterIndex - (int)startLineIndex > 0) { //cut line at the middle of a word
                        cutTextLines.emplace_back(TextLine{.text = u32Text.substr(startLineIndex, letterIndex - startLineIndex), .cutType = TextCutType::MIDDLE_WORD});
                        startLineIndex = letterIndex;
                        lineLength = letterLength;
                        lengthFromLastSpace = letterLength;
                    }
                } else {
                    lineLength += letterLength;
                    lengthFromLastSpace += letterLength;
                }
            }
        } else {
            for (std::size_t letterIndex = 0; letterIndex < u32Text.length(); letterIndex++) { //each letters
                if (u32Text[letterIndex] == '\n') {
                    cutTextLines.emplace_back(TextLine{.text = u32Text.substr(startLineIndex, letterIndex - startLineIndex), .cutType = TextCutType::CLASSIC});
                    startLineIndex = letterIndex + 1;
                }
            }
        }

        if (startLineIndex == 0) {
            cutTextLines.emplace_back(TextLine{.text = u32Text, .cutType = TextCutType::CLASSIC});
        } else if ((int)u32Text.length() - (int)startLineIndex >= 0) {
            cutTextLines.emplace_back(TextLine{.text = u32Text.substr(startLineIndex, u32Text.length() - startLineIndex), .cutType = TextCutType::CLASSIC});
        }
    }

    void Text::refreshFont() {
        auto textChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "text", UdaAttribute("skin", skinName));
        std::string ttfFilename = UISkinService::instance().getSkinReader().getFirstChunk(true, "font", UdaAttribute(), textChunk)->getStringValue();
        std::string fontColor = UISkinService::instance().getSkinReader().getFirstChunk(true, "color", UdaAttribute(), textChunk)->getStringValue();
        unsigned int fontHeight = retrieveFontHeight(textChunk);

        std::map<std::string, std::string, std::less<>> fontParams = {{"fontSize", std::to_string(fontHeight)}, {"fontColor", fontColor}};
        font = ResourceRetriever::instance().getResource<Font>(ttfFilename, fontParams);
        updateTexture(font->getTexture());
    }

    unsigned int Text::retrieveFontHeight(const UdaChunk* textChunk) const {
        Length fontHeight = UISkinService::instance().loadLength(textChunk, "height");
        if (fontHeight.type == LengthType::PIXEL) {
            return MathFunction::roundToUInt(fontHeight.value);
        } else if (fontHeight.type == LengthType::SCREEN_PERCENT) {
            return MathFunction::roundToUInt(fontHeight.value / 100.0f * (float)getSceneSize().Y);
        }
        throw std::runtime_error("Unimplemented length type for font height: " + std::to_string(fontHeight.type));
    }

}
