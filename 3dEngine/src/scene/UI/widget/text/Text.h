#ifndef URCHINENGINE_TEXT_H
#define URCHINENGINE_TEXT_H

#include <string>
#include <memory>

#include "scene/UI/widget/Widget.h"
#include "scene/UI/widget/Position.h"
#include "scene/UI/widget/Length.h"
#include "resources/font/Font.h"
#include "graphic/render/GenericRenderer.h"

namespace urchin {

    class Text : public Widget {
        public:
            Text(Widget*, Position, std::string, std::string);
            Text(Position, std::string, std::string);
            ~Text() override;

            void setMaxWidth(Length);
            void updateText(const std::string&);

            const std::string& getText() const;
            const Font* getFont();

        protected:
            void createOrUpdateWidget() override;
            void prepareWidgetRendering(float) override;

        private:
            unsigned int getMaxWidth();

            void refreshTextAndWidgetSize();
            std::string cutText(const std::string&);
            void refreshFont();
            unsigned int retrieveFontHeight(const std::shared_ptr<XmlChunk>&) const;
            static LengthType toLengthType(const std::string&);
            void cleanFont();
            void refreshRenderer();

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
            std::unique_ptr<GenericRenderer> textRenderer;
    };

}

#endif
