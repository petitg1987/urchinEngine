#pragma once

#include <string>
#include <memory>
#include <codecvt>
#include <optional>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/allocator/StringConverterAllocator.h>
#include <i18n/TranslatableLabel.h>
#include <resources/font/Font.h>
#include <api/render/GenericRenderer.h>

namespace urchin {

    constexpr char TRANSLATABLE_TEXT_PREFIX = '~';
    constexpr auto i18n = [](const std::string& text) { return TRANSLATABLE_TEXT_PREFIX + text; };

    class Text : public Widget, public TranslatableLabel {
        public:
            static std::shared_ptr<Text> create(Widget*, Position, std::string, std::string);
            static std::shared_ptr<Text> createTranslatable(Widget*, Position, std::string, const std::string&);
            ~Text() override;

            void setMaxWidth(float, LengthType);
            void updateText(std::string);
            void updateLabelKey(std::string);

            const std::string& getText() const;
            const Font& getFont() const;

        protected:
            void createOrUpdateWidget() override;

            void refreshTranslation(const LanguageTranslator&&) override;

            void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) override;

        private:
            Text(Position, std::string, std::string);

            unsigned int getMaxWidth() const;
            bool hasTranslatableText() const;

            void refreshTextAndWidgetSize();
            void cutText();
            void refreshFont();
            unsigned int retrieveFontHeight(const UdaChunk*) const;
            void refreshCoordinates();
            void refreshRenderer();
            void refreshRendererData();

            //properties
            std::string skinName;
            std::vector<std::string> inputTexts;
            std::string text;
            float maxWidth;
            LengthType maxWidthType;

            //data
            WStringConvertA stringConvert;
            std::vector<U32StringA> cutTextLines;
            std::shared_ptr<Font> font;

            //visual
            std::vector<Point2<float>> vertexCoord;
            std::vector<Point2<float>> textureCoord;
            std::unique_ptr<GenericRenderer> textRenderer;
    };

}
