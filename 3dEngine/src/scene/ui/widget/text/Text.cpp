#include <memory>
#include <utility>

#include <scene/ui/widget/text/Text.h>
#include <scene/ui/widget/container/Container.h>
#include <scene/ui/widget/Size.h>
#include <scene/ui/UISkinService.h>
#include <resources//ResourceRetriever.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    Text::Text(Position position, std::string skinName, std::string inputText, std::vector<std::string> inputTextParameters) :
            Widget(position, Size(0, 0, LengthType::PIXEL)),
            skinName(std::move(skinName)),
            inputText(std::move(inputText)),
            inputTextParameters(std::move(inputTextParameters)),
            parameterRegex(std::regex("\\{[a-zA-Z-]+}", std::regex_constants::optimize)),
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
        if (hasTranslatableInput()) {
            getI18nService().remove(this);
        }
    }

    void Text::createOrUpdateWidget() {
        refreshFont();

        if (hasTranslatableInput()) {
            getI18nService().add(this);
        } else {
            baseText = evaluateText(std::nullopt);
            refreshTextAndWidgetSize();
        }
        refreshRenderer();
    }

    WidgetType Text::getWidgetType() const {
        return WidgetType::TEXT;
    }

    void Text::updateSize(Size) {
        throw std::runtime_error("Update size is not allowed on text: " + inputText);
    }

    void Text::setMaxWidth(float maxWidth, LengthType maxWidthType) {
        this->maxWidth = maxWidth;
        this->maxWidthType = maxWidthType;

        if (isInitialized()) {
            refreshTextAndWidgetSize();
            refreshRendererData();
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

    bool Text::hasTranslatableInput() const {
        if (!inputText.empty() && inputText[0] == TRANSLATABLE_TEXT_PREFIX) {
            return true;
        }

        if (!inputTextParameters.empty()) {
            bool hasTranslatableParameters = std::ranges::any_of(inputTextParameters, [](const std::string& parameter) {
                return !parameter.empty() && parameter[0] == TRANSLATABLE_TEXT_PREFIX;
            });
            if (hasTranslatableParameters) {
                return true;
            }
        }

        return false;
    }

    void Text::updateText(std::string inputText) {
        updateText(std::move(inputText), {});
    }

    void Text::updateText(const ParameterizedText& parameterizedText) {
        updateText(parameterizedText.getText(), parameterizedText.getParameters());
    }

    void Text::updateText(std::string inputText, std::vector<std::string> inputTextParameters) {
        if (hasTranslatableInput()) {
            getI18nService().remove(this);
        }

        this->inputText = std::move(inputText);
        this->inputTextParameters = std::move(inputTextParameters);

        if (!hasTranslatableInput()) {
            baseText = evaluateText(std::nullopt);
            refreshTextAndWidgetSize();
            refreshRendererData();
        } else {
            getI18nService().add(this);
        }
    }

    std::string Text::evaluateText(const std::optional<LanguageTranslator>& languageTranslator) const {
        std::string evaluatedText;
        if (!inputText.empty() && inputText[0] == TRANSLATABLE_TEXT_PREFIX) {
            if (!languageTranslator.has_value()) {
                throw std::runtime_error("Language translator missing for text: " + inputText);
            }
            evaluatedText = languageTranslator->translate(std::string_view(inputText).substr(1));
        } else {
            evaluatedText = inputText;
        }

        for (const std::string& inputTextParameter : inputTextParameters) {
            std::string paramValue;
            if (!inputTextParameter.empty() && inputTextParameter[0] == TRANSLATABLE_TEXT_PREFIX) {
                if (!languageTranslator.has_value()) {
                    throw std::runtime_error("Language translator missing for text: " + inputText);
                }
                paramValue = languageTranslator->translate(std::string_view(inputTextParameter).substr(1));
            } else {
                paramValue = inputTextParameter;
            }

            evaluatedText = std::regex_replace(evaluatedText, parameterRegex, paramValue, std::regex_constants::format_first_only);
        }

        return evaluatedText;
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
        baseText = evaluateText(std::make_optional(languageTranslator));

        refreshTextAndWidgetSize();
        refreshRendererData();
    }

    const Font& Text::getFont() const {
        return *font;
    }

    void Text::refreshTextAndWidgetSize() {
        //cut the text if needed
        cutText();

        //compute widget size
        float width = 0.0f;
        auto spaceBetweenLetters = (float)font->getSpaceBetweenLetters();
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
        setSize(Size(width, textHeight, LengthType::PIXEL));
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

                unsigned int letterLength = font->getGlyph(textLetter).width + font->getSpaceBetweenLetters();

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

    void Text::refreshCoordinates() {
        //creates the vertex array and texture array
        vertexCoord.clear();
        textureCoord.clear();
        vertexCoord.reserve(baseText.size() * 4);
        textureCoord.reserve(baseText.size() * 4);

        float offsetY = 0.0f;
        auto spaceBetweenLetters = (float)font->getSpaceBetweenLetters();
        auto spaceBetweenLines = (float)font->getSpaceBetweenLines();

        for (const TextLine& textLine : cutTextLines) { //each line
            float offsetX = 0.0f;
            for (char32_t textLetter : textLine.text) { //each letter
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

        if (renderer) {
            renderer->updateData(0, vertexCoord);
            renderer->updateData(1, textureCoord);
        }
    }

    void Text::refreshRenderer() {
        std::string renderName = inputText.substr(0, std::min((std::size_t)10, inputText.size()));
        renderer = setupUiRenderer("text_" + renderName, ShapeType::TRIANGLE, true)
                ->addUniformTextureReader(TextureReader::build(font->getTexture(), TextureParam::build(TextureParam::EDGE_CLAMP, TextureParam::LINEAR, getTextureAnisotropy()))) //binding 3
                ->build();
    }

    void Text::refreshRendererData() {
        if (renderer != nullptr) {
            refreshCoordinates();

            renderer->updateData(0, vertexCoord);
            renderer->updateData(1, textureCoord);
        }
    }

    void Text::prepareWidgetRendering(float, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        updateProperties(renderer.get(), projectionViewMatrix, Vector2<float>(getGlobalPositionX(), getGlobalPositionY()));
        renderer->enableRenderer(renderingOrder);
    }

}
