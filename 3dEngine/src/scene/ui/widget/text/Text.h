#pragma once

#include <string>
#include <memory>
#include <codecvt>
#include <optional>
#include <regex>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/text/ParameterizedText.h>
#include <scene/ui/allocator/StringConverterAllocator.h>
#include <i18n/TranslatableLabel.h>
#include <resources/font/Font.h>
#include <graphics/api/GraphicsApi.h>

namespace urchin {

    enum class TextCutType {
        CLASSIC, //text is cut because of '\n' presence or because it is the last line
        WORD, //text is too long and is cut between two words
        MIDDLE_WORD //text is too long and is cut at the middle of a word
    };

    struct TextLine {
        U32StringA text;
        TextCutType cutType; //reason of line is cut at the end
    };

    class Text : public Widget, public TranslatableLabel {
        public:
            static constexpr char TRANSLATABLE_TEXT_PREFIX = '~';

            static std::shared_ptr<Text> create(Widget*, Position, std::string, std::string);
            static std::shared_ptr<Text> create(Widget*, Position, std::string, const ParameterizedText&);
            ~Text() override;

            WidgetType getWidgetType() const override;
            void updateSize(Size) override;

            void setMaxWidth(float, LengthType);
            void updateText(std::string);
            void updateText(const ParameterizedText&);

            const std::string& getBaseText() const;
            const std::vector<TextLine>& getCutTextLines() const;
            std::size_t cutTextToBaseTextIndex(std::size_t) const;
            std::size_t baseTextToCutTextIndex(std::size_t) const;
            const Font& getFont() const;

        protected:
            void createOrUpdateWidget() override;

            void refreshTranslation(const LanguageTranslator&) override;

            void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) override;

        private:
            Text(Position, std::string, std::string, std::vector<std::string>);

            void updateText(std::string, std::vector<std::string>);
            std::string evaluateText(const std::optional<LanguageTranslator>&) const;
            int getMaxWidthPixel() const;
            bool hasTranslatableInput() const;

            void refreshTextAndWidgetSize();
            void cutText();
            void refreshFont();
            unsigned int retrieveFontHeight(const UdaChunk*) const;
            void refreshCoordinates();
            void refreshRenderer();
            void refreshRendererData();

            //properties
            std::string skinName;
            std::string inputText;
            std::vector<std::string> inputTextParameters;
            std::regex parameterRegex;
            std::string baseText;
            float maxWidth;
            LengthType maxWidthType;

            //data
            WStringConvertA stringConvert;
            std::vector<TextLine> cutTextLines;
            std::shared_ptr<Font> font;

            //visual
            std::vector<Point2<float>> vertexCoord;
            std::vector<Point2<float>> textureCoord;
            std::unique_ptr<GenericRenderer> textRenderer;
    };

    constexpr auto i18n = [](const std::string& text) { return Text::TRANSLATABLE_TEXT_PREFIX + text; };

}
