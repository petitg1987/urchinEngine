#pragma once

#include <string>
#include <memory>
#include <codecvt>
#include <optional>
#include <regex>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/allocator/StringConverterAllocator.h>
#include <i18n/ParameterizedText.h>
#include <i18n/TranslatableLabel.h>
#include <resources/font/Font.h>
#include <graphics/api/GraphicsApi.h>

namespace urchin {

    enum class TextCutType {
        CLASSIC, //text is cut because of '\n' presence or because it is the last line
        WORD, //text is too long and is cut between two words
        MIDDLE_WORD //text is too long and is cut at the middle of a word
    };

    enum class WordCutIndexPositioning {
        //When a word is cut at the middle (TextCutType#MIDDLE_WORD), the index at end of line and at begin of line represents the same index in the baseText.
        //This enum allows to define the expected index in the cut text when the index is converted from the base text
        END_OF_LINE,
        BEGIN_OF_NEXT_LINE
    };

    struct TextLine {
        U32StringA text;
        TextCutType cutType; //reason of line is cut at the end
    };

    class Text final : public Widget, public TranslatableLabel {
        public:
            static std::shared_ptr<Text> create(Widget*, Position, std::string, std::string);
            static std::shared_ptr<Text> create(Widget*, Position, std::string, const ParameterizedText&);
            ~Text() override;

            WidgetType getWidgetType() const override;
            void updateSize(Size) override;

            void setMaxWidth(float, LengthType);
            void updateText(std::string);
            void updateText(ParameterizedText);

            const std::string& getBaseText() const;
            const std::vector<TextLine>& getCutTextLines() const;
            std::size_t cutTextIndexToBaseTextIndex(std::size_t) const;
            std::size_t baseTextIndexToCutTextIndex(std::size_t, WordCutIndexPositioning = WordCutIndexPositioning::BEGIN_OF_NEXT_LINE) const;
            std::size_t baseTextIndexToEndOfLineIndex(std::size_t, WordCutIndexPositioning = WordCutIndexPositioning::BEGIN_OF_NEXT_LINE) const;
            const Font& getFont() const;

            void retrieveVertexCoordinates(std::vector<Point2<float>>&) override;
            void retrieveTextureCoordinates(std::vector<Point2<float>>&) override;

        private:
            Text(Position, std::string, std::string, std::vector<std::string>);

            void createOrUpdateWidget() override;
            bool requireRenderer() const override;
            void uninitialize() override;

            int getMaxWidthPixel() const;

            void refreshTranslation(const LanguageTranslator&) override;
            void refreshTextAndWidgetSize();
            void cutText();
            void refreshFont();
            unsigned int retrieveFontHeight(const UdaChunk*) const;

            //properties
            std::string skinName;
            ParameterizedText inputText;
            std::string baseText;
            float maxWidth;
            LengthType maxWidthType;

            //data
            WStringConvertA stringConvert;
            std::vector<TextLine> cutTextLines;
            std::shared_ptr<Font> font;
    };

}
