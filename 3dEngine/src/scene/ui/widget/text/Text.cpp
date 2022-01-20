#include <memory>
#include <utility>

#include <scene/ui/widget/text/Text.h>
#include <scene/ui/widget/container/Container.h>
#include <scene/ui/widget/Size.h>
#include <scene/ui/UISkinService.h>
#include <resources//ResourceRetriever.h>
#include <api/render/GenericRendererBuilder.h>

namespace urchin {

    Text::Text(Position position, std::string skinName, std::string inputText, std::vector<std::string> inputTextParameters) :
            Widget(position, Size(0, 0, LengthType::PIXEL)),
            skinName(std::move(skinName)),
            inputText(std::move(inputText)),
            inputTextParameters(std::move(inputTextParameters)),
            parameterRegex("\\{[a-zA-Z-]+}"),
            maxWidth(100.0f),
            maxWidthType(LengthType::SCREEN_PERCENT),
            font(nullptr) {

    }

    std::shared_ptr<Text> Text::create(Widget* parent, Position position, std::string skinName, std::string inputText) {
        return Widget::create<Text>(new Text(position, std::move(skinName), std::move(inputText), {}), parent);
    }

    std::shared_ptr<Text> Text::create(Widget* parent, Position position, std::string skinName, const ParameterizedText& parameterizedText) {
        return Widget::create<Text>(new Text(position, std::move(skinName), parameterizedText.getText(), parameterizedText.getParameters()), parent);
    }

    Text::~Text() {
        if (hasTranslatableInput() && getI18nService()) {
            getI18nService()->remove(this);
        }
    }

    void Text::createOrUpdateWidget() {
        refreshFont();

        if (hasTranslatableInput()) {
            getI18nService()->add(this);
        } else {
            text = evaluateText(std::nullopt);
            refreshTextAndWidgetSize();
        }
        refreshRenderer();
    }

    void Text::setMaxWidth(float maxWidth, LengthType maxWidthType) {
        this->maxWidth = maxWidth;
        this->maxWidthType = maxWidthType;

        if (isInitialized()) {
            refreshTextAndWidgetSize();
            refreshRendererData();
        }
    }

    unsigned int Text::getMaxWidth() const {
        if (maxWidthType == LengthType::PIXEL) {
            return (unsigned int)maxWidth;
        } else if (maxWidthType == LengthType::SCREEN_PERCENT) {
            return (unsigned int)(maxWidth / 100.0f * (float)getSceneSize().X);
        } else if (maxWidthType == LengthType::CONTAINER_PERCENT) {
            if (!getParentContainer()) {
                throw std::runtime_error("Missing parent container on the widget");
            }
            return (unsigned int)(maxWidth / 100.0f * (float)getParentContainer()->getWidth());
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
            getI18nService()->remove(this);
        }

        this->inputText = std::move(inputText);
        this->inputTextParameters = std::move(inputTextParameters);

        if (!hasTranslatableInput()) {
            text = evaluateText(std::nullopt);
            refreshTextAndWidgetSize();
            refreshRendererData();
        } else {
            getI18nService()->add(this);
        }
    }

    std::string Text::evaluateText(const std::optional<LanguageTranslator>& languageTranslator) const {
        std::string evaluatedText = inputText;
        if (!evaluatedText.empty() && evaluatedText[0] == TRANSLATABLE_TEXT_PREFIX) {
            if (!languageTranslator.has_value()) {
                throw std::runtime_error("Language translator missing for text: " + inputText);
            }
            evaluatedText = languageTranslator->translate(evaluatedText.substr(1));
        }

        for (const std::string& inputTextParameter : inputTextParameters) {
            std::string paramValue = inputTextParameter;
            if (!paramValue.empty() && paramValue[0] == TRANSLATABLE_TEXT_PREFIX) {
                if (!languageTranslator.has_value()) {
                    throw std::runtime_error("Language translator missing for text: " + inputText);
                }
                paramValue = languageTranslator->translate(paramValue.substr(1));
            }

            evaluatedText = std::regex_replace(evaluatedText, parameterRegex, paramValue, std::regex_constants::format_first_only);
        }

        return evaluatedText;
    }

    const std::string& Text::getText() const {
        return text;
    }

    void Text::refreshTranslation(const LanguageTranslator& languageTranslator) {
        text = evaluateText(std::make_optional(languageTranslator));

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
            for (char32_t textLetter : textLine) { //each letter
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
        U32StringA u32Text = stringConvert.from_bytes(text.c_str());

        std::size_t startLineIndex = 0;

        if (getMaxWidth() != 0) {
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

                if (lineLength + letterLength >= getMaxWidth()) { //cut too long line
                    if ((int)lastSpaceIndex - (int)startLineIndex > 0) { //cut line at last space found
                        cutTextLines.push_back(u32Text.substr(startLineIndex, lastSpaceIndex - startLineIndex));
                        startLineIndex = lastSpaceIndex + 1;
                        lineLength = lengthFromLastSpace;
                    } else if ((int)letterIndex - (int)startLineIndex > 0) { //cut line at the middle of a word
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
        } else {
            for (std::size_t letterIndex = 0; letterIndex < u32Text.length(); letterIndex++) { //each letters
                if (u32Text[letterIndex] == '\n') {
                    cutTextLines.emplace_back(u32Text.substr(startLineIndex, letterIndex - startLineIndex));
                    startLineIndex = letterIndex + 1;
                }
            }
        }

        if (startLineIndex == 0 && u32Text.length() > 0) {
            cutTextLines.emplace_back(u32Text);
        } else if ((int)u32Text.length() - (int)startLineIndex > 0) {
            cutTextLines.emplace_back(u32Text.substr(startLineIndex, u32Text.length() - startLineIndex));
        }
    }

    void Text::refreshFont() {
        auto textChunk = UISkinService::instance().getSkinReader().getUniqueChunk(true, "text", UdaAttribute("skin", skinName));
        std::string ttfFilename = UISkinService::instance().getSkinReader().getUniqueChunk(true, "font", UdaAttribute(), textChunk)->getStringValue();
        std::string fontColor = UISkinService::instance().getSkinReader().getUniqueChunk(true, "color", UdaAttribute(), textChunk)->getStringValue();
        unsigned int fontHeight = retrieveFontHeight(textChunk);

        std::map<std::string, std::string> fontParams = {{"fontSize", std::to_string(fontHeight)}, {"fontColor", fontColor}};
        font = ResourceRetriever::instance().getResource<Font>(ttfFilename, fontParams);
    }

    unsigned int Text::retrieveFontHeight(const UdaChunk* textChunk) const {
        LengthType fontHeightType;
        float fontHeight = UISkinService::instance().loadLength(textChunk, "height", fontHeightType);
        if (fontHeightType == LengthType::PIXEL) {
            return (unsigned int)fontHeight;
        } else if (fontHeightType == LengthType::SCREEN_PERCENT) {
            return (unsigned int)(fontHeight / 100.0f * (float)getSceneSize().Y);
        }
        throw std::runtime_error("Unimplemented length type for font height: " + std::to_string(fontHeightType));
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

        for (const auto& textLine : cutTextLines) { //each line
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

        std::string renderName = inputText.substr(0, std::min((std::size_t)10, inputText.size()));
        textRenderer = setupUiRenderer("text_" + renderName, ShapeType::TRIANGLE, true)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformTextureReader(TextureReader::build(font->getTexture(), TextureParam::buildLinear())) //binding 2
                ->build();
    }

    void Text::refreshRendererData() {
        if (textRenderer != nullptr) {
            refreshCoordinates();

            textRenderer->updateData(0, vertexCoord);
            textRenderer->updateData(1, textureCoord);
        }
    }

    void Text::prepareWidgetRendering(float, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        updatePositioning(textRenderer.get(), projectionViewMatrix, Vector2<int>(getGlobalPositionX(), getGlobalPositionY()));
        textRenderer->enableRenderer(renderingOrder);
    }

}
