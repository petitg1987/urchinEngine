#pragma once

#include <vector>
#include <unordered_map>

#include <scene/ui/UIRenderer.h>

namespace urchin {

    class WidgetSetDisplayer final : public Observer {
        public:
            explicit WidgetSetDisplayer(const UIRenderer&);
            ~WidgetSetDisplayer() override;

            void onUiRendererSizeUpdated()  const;
            void onGammaFactorUpdated() const;
            void notify(Observable*, int) override;

            void updateWidgets(std::span<Widget* const>);
            void removeWidget(Widget*);
            std::span<Widget* const> getWidgets() const;

            void prepareRendering(unsigned int&, const Matrix4<float>&, const Vector2<float>&);

        private:
            WidgetInstanceDisplayer* findWidgetInstanceDisplayer(const Widget&) const;
            void clearDisplayers();
            void removeWidgetFromDisplayer(Widget&, WidgetInstanceDisplayer&);
            void addWidgetToDisplayer(Widget&, WidgetInstanceDisplayer&);

            void observeWidgetUpdate(Widget&);
            void unobserveWidgetUpdate(Widget&);

            void cleanUnusedDisplayers();

            const UIRenderer& uiRenderer;

            std::vector<Widget*> widgets;

            EverGrowHashSet<WidgetInstanceDisplayer*> activeInstanceDisplayers;
            std::vector<WidgetInstanceDisplayer*> activeSortedInstanceDisplayers;
            std::unordered_map<Widget*, std::unique_ptr<WidgetInstanceDisplayer>> exclusiveInstanceDisplayers;
            std::unordered_map<std::size_t, std::unique_ptr<WidgetInstanceDisplayer>> shareableInstanceDisplayers;
    };

}
