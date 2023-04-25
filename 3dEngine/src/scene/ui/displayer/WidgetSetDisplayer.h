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
            const std::vector<Widget*>& getWidgets() const;

            void prepareRendering(unsigned int&, const Matrix4<float>&);

        private:
            WidgetInstanceDisplayer* findWidgetInstanceDisplayer(const Widget&) const;
            void clearDisplayers();
            void removeWidgetFromDisplayer(Widget&, WidgetInstanceDisplayer&) const;
            void addWidgetToDisplayer(Widget&, WidgetInstanceDisplayer&) const;

            const UIRenderer& uiRenderer;

            std::vector<Widget*> widgets;

            //use combination of std::unordered_set for uniqueness and a std::vector to keep the insertion order
            std::unordered_set<WidgetInstanceDisplayer*> activeWidgetDisplayers;
            std::vector<WidgetInstanceDisplayer*> activeWidgetDisplayersOrdered;

            std::unordered_map<Widget*, std::unique_ptr<WidgetInstanceDisplayer>> widgetDisplayers;
            std::unordered_map<std::size_t, std::unique_ptr<WidgetInstanceDisplayer>> widgetInstanceDisplayers;
    };

}