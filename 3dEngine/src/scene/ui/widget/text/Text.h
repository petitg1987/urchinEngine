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

    class Text : public Widget, public TranslatableLabel {
        public:
            static std::shared_ptr<Text> create(Widget*, Position, std::string, std::string);
            static std::shared_ptr<Text> createTranslatable(Widget*, Position, std::string, std::string);
            ~Text() override;

            void setMaxWidth(float, LengthType);
            void updateText(std::string);
            void updateLabelKey(const std::string&);

            const std::string& getText() const;
            const Font& getFont() const;

        protected:
            void createOrUpdateWidget() override;

            bool isTranslatableLabel() const;
            void refreshTranslation(const LanguageTranslator&&) override;

            void prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) override;

        private:
            Text(Position, std::string, std::string, bool);

            unsigned int getMaxWidth() const;

            void refreshTextAndWidgetSize();
            void cutText();
            void refreshFont();
            unsigned int retrieveFontHeight(const UdaChunk*) const;
            void refreshCoordinates();
            void refreshRenderer();
            void refreshRendererData();

            //properties
            std::string skinName;
            std::string text;
            std::optional<std::string> labelKey;
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
