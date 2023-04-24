#pragma once

#include <scene/ui/widget/Widget.h>

namespace urchin {

    class WidgetInstanceDisplayer {
        public:
            explicit WidgetInstanceDisplayer(const UIRenderer&);
            ~WidgetInstanceDisplayer();

            void initialize(std::shared_ptr<Texture>);
            void updateTexture(std::shared_ptr<Texture>);

            void addInstanceWidget(Widget&);
            void removeInstanceWidget(Widget&);
            unsigned int getInstanceCount() const;

            void prepareRendering(unsigned int, const Matrix4<float>&) const;

        private:
            Widget& getReferenceWidget() const;
            unsigned int computeDepthLevel(Widget&) const;
            TextureParam::Anisotropy getTextureAnisotropy() const;

            bool isInitialized;

            std::vector<Widget*> instanceWidgets;
            std::size_t instanceId;

            const UIRenderer& uiRenderer;

            struct ColorParams {
                alignas(4) float alphaFactor;
                alignas(4) float gammaFactor;
            };
            ColorParams colorParams;
            mutable std::vector<Matrix4<float>> instanceModelMatrices;

            std::unique_ptr<GenericRenderer> renderer;
    };

}