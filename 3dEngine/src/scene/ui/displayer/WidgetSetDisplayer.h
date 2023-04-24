#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <scene/ui/UIRenderer.h>

namespace urchin {

    class WidgetSetDisplayer {
        public:
            explicit WidgetSetDisplayer(const UIRenderer&);
            ~WidgetSetDisplayer();

            void updateWidgets(std::span<Widget* const>);
            void removeWidget(Widget*);

            void prepareRendering(unsigned int, const Matrix4<float>&);

        private:
            WidgetInstanceDisplayer* findWidgetInstanceDisplayer(const Widget&) const;
            void clearDisplayers();
            void removeWidgetFromDisplayer(Widget&, WidgetInstanceDisplayer&) const;
            void addWidgetToDisplayer(Widget&, WidgetInstanceDisplayer&) const;

            const UIRenderer& uiRenderer;

            std::vector<Widget*> widgets;
            std::unordered_set<WidgetInstanceDisplayer*> activeWidgetDisplayers;
            std::unordered_map<Widget*, std::unique_ptr<WidgetInstanceDisplayer>> widgetDisplayers;
            std::unordered_map<std::size_t, std::unique_ptr<WidgetInstanceDisplayer>> widgetInstanceDisplayers;
    };

}