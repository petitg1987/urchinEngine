#pragma once

#include <string>
#include <memory>

#include <scene/UI/widget/Widget.h>
#include <scene/UI/widget/Position.h>
#include <scene/UI/widget/Length.h>
#include <resources/font/Font.h>
#include <graphic/render/GenericRenderer.h>

namespace urchin {

    class Text : public Widget {
        public:
            static Text* newText(Widget*, Position, std::string, std::string);
            static Text* newTranslatableText(Widget*, Position, std::string, std::string);
            ~Text() override;

            void setMaxWidth(Length);
            void updateText(const std::string&);

            const std::string& getText() const;
            const Font* getFont();

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float) override;

        private:
            Text(Widget*, Position, std::string, std::string);

            unsigned int getMaxWidth();

            void refreshTextAndWidgetSize();
            std::string cutText(const std::string&);
            void refreshFont();
            unsigned int retrieveFontHeight(const std::shared_ptr<XmlChunk>&) const;
            LengthType toLengthType(const std::string&) const;
            void cleanFont();
            void refreshCoordinates();
            void refreshRenderer();
            void refreshRendererData();

            //properties
            std::string nameSkin;
            std::string text;
            Length maxWidth;

            //data
            std::vector<std::string> cutTextLines;
            Font* font;

            //visual
            std::vector<Point2<float>> vertexCoord;
            std::vector<Point2<float>> textureCoord;
            std::shared_ptr<GenericRenderer> textRenderer;
    };

}
