#pragma once

#include <scene/ui/widget/Widget.h>

namespace urchin {

    class WidgetSetDisplayer;

    class WidgetInstanceDisplayer {
        public:
            explicit WidgetInstanceDisplayer(const WidgetSetDisplayer&, const UIRenderer&);
            ~WidgetInstanceDisplayer();

            void initialize();

            void onUiRendererSizeUpdated();
            void onGammaFactorUpdated();
            void updateTexture(std::size_t);
            void updateScissor(std::size_t);
            void updateCoordinates(std::size_t);
            void updateAlphaFactor(std::size_t);

            const WidgetSetDisplayer& getWidgetSetDisplayer() const;
            std::size_t getInstanceId() const;
            const std::vector<Widget*>& getInstanceWidgets() const;

            void addInstanceWidget(Widget&);
            void removeInstanceWidget(Widget&);
            unsigned int getInstanceCount() const;

            void resetRenderingWidgets();
            void registerRenderingWidget(const Widget&);
            void prepareRendering(unsigned int, const Matrix4<float>&) const;

        private:
            Widget& getReferenceWidget() const;
            TextureParam::Anisotropy getTextureAnisotropy() const;

            bool isInitialized;

            const WidgetSetDisplayer& widgetSetDisplayer;
            std::vector<Widget*> instanceWidgets;
            std::size_t instanceId;

            const UIRenderer& uiRenderer;

            std::vector<Point2<float>> coordinates;
            struct ColorParams {
                alignas(4) float alphaFactor;
                alignas(4) float gammaFactor;
            };
            ColorParams colorParams;
            mutable std::vector<Matrix4<float>> instanceModelMatrices;

            std::unique_ptr<GenericRenderer> renderer;
    };

}