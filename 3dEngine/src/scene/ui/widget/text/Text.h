#pragma once

#include <string>
#include <memory>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/Position.h>
#include <scene/ui/widget/Length.h>
#include <i18n/TranslatableLabel.h>
#include <resources/font/Font.h>
#include <graphic/render/GenericRenderer.h>

namespace urchin {

    class Text : public Widget, public TranslatableLabel {
        public:
            static std::shared_ptr<Text> create(Widget*, Position, std::string, std::string);
            static std::shared_ptr<Text> createTranslatable(Widget*, Position, std::string, std::string);
            ~Text() override;

            void setMaxWidth(Length);
            void updateText(const std::string&);

            bool isTranslatableLabel() const;
            const std::string& getText() const;
            const Font& getFont();

        protected:
            void createOrUpdateWidget() override;

            const std::string& getLabelKey() const override;
            void updateLabel(const std::string&) override;

            void prepareWidgetRendering(float) override;

        private:
            Text(Position, std::string, std::string, bool);

            unsigned int getMaxWidth();

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
            Length maxWidth;

            //data
            std::vector<std::u32string> cutTextLines;
            std::shared_ptr<Font> font;

            //visual
            std::vector<Point2<float>> vertexCoord;
            std::vector<Point2<float>> textureCoord;
            std::unique_ptr<GenericRenderer> textRenderer;
    };

}
