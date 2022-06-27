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
#include <api/render/GenericRenderer.h>

namespace urchin {

    struct TextLine {
        U32StringA text;
        bool spaceIntoLineFeed;
    };

    class Text : public Widget, public TranslatableLabel {
        public:
            static constexpr char TRANSLATABLE_TEXT_PREFIX = '~';

            static std::shared_ptr<Text> create(Widget*, Position, std::string, std::string);
            static std::shared_ptr<Text> create(Widget*, Position, std::string, const ParameterizedText&);
            ~Text() override;

            WidgetType getWidgetType() const override;

            void setMaxWidth(float, LengthType);
            void updateText(std::string);
            void updateText(const ParameterizedText&);

            const std::string& getText() const;
            const std::vector<TextLine>& getCutTextLines() const;
            const Font& getFont() const;

        protected:
            void createOrUpdateWidget() override;

            void refreshTranslation(const LanguageTranslator&) override;

            void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) override;

        private:
            Text(Position, std::string, std::string, std::vector<std::string>);

            void updateText(std::string, std::vector<std::string>);
            std::string evaluateText(const std::optional<LanguageTranslator>&) const;
            unsigned int getMaxWidth() const;
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
            std::string text;
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
