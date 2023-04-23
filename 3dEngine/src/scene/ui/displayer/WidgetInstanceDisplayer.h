#pragma once

#include <scene/ui/widget/Widget.h>

namespace urchin {

    class WidgetInstanceDisplayer {
        public:
            WidgetInstanceDisplayer(UIRenderer&, RenderTarget&, const Shader&);
            ~WidgetInstanceDisplayer();

            void initialize();

            void addInstanceWidget(Widget&);
            void removeInstanceWidget(Widget&);
            unsigned int getInstanceCount() const;

            void prepareRendering(unsigned int, const Matrix4<float>&) const;

        private:
            Widget& getReferenceWidget() const;

            bool isInitialized;

            std::vector<Widget*> instanceWidgets;
            std::size_t instanceId;

            UIRenderer& uiRenderer;
            RenderTarget& renderTarget;
            const Shader& shader;

            std::unique_ptr<GenericRenderer> renderer;
    };

}