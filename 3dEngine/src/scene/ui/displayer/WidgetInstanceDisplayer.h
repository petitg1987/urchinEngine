#pragma once

#include <scene/ui/widget/Widget.h>

namespace urchin {

    class WidgetSetDisplayer;

    class WidgetInstanceDisplayer : public Observer {
        public:
            explicit WidgetInstanceDisplayer(const WidgetSetDisplayer&, const UIRenderer&);
            ~WidgetInstanceDisplayer() override;

            void initialize();
            void notify(Observable*, int) override;

            void onUiRendererSizeUpdated();
            void onGammaFactorUpdated();

            const WidgetSetDisplayer& getWidgetSetDisplayer() const;
            std::size_t getInstanceId() const;

            void addInstanceWidget(Widget&);
            void removeInstanceWidget(Widget&);
            unsigned int getInstanceCount() const;

            void resetRenderingWidgets();
            void registerRenderingWidget(const Widget&);
            void prepareRendering(unsigned int, const Matrix4<float>&) const;

        private:
            Widget& getReferenceWidget() const;
            TextureParam::Anisotropy getTextureAnisotropy() const;

            bool checkUpdateAllowance() const;
            void updateTexture();
            void updateScissor();
            void updateCoordinates();
            void updateAlphaFactor();

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