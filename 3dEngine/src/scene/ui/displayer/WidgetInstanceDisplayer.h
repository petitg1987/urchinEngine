#pragma once

#include "scene/ui/widget/Widget.h"

namespace urchin {

    class WidgetSetDisplayer;

    class WidgetInstanceDisplayer {
        public:
            explicit WidgetInstanceDisplayer(const WidgetSetDisplayer&, const UIRenderer&);
            ~WidgetInstanceDisplayer();

            void initialize();

            void onUiRendererSizeUpdated() const;
            void onGammaFactorUpdated();
            void updateTexture() const;
            void updateScissor() const;
            void updateCoordinates();
            void updateAlphaFactor();

            const WidgetSetDisplayer& getWidgetSetDisplayer() const;
            std::size_t getInstanceId() const;
            void updateInstanceId(std::size_t);
            std::span<Widget* const> getInstanceWidgets() const;

            void addInstanceWidget(Widget&);
            void removeInstanceWidget(Widget&);
            unsigned int getInstanceCount() const;
            std::chrono::steady_clock::time_point getLastRenderingTime() const;

            void resetRenderingWidgets() const;
            void registerRenderingWidget(const Widget&) const;
            void prepareRendering(unsigned int, const Matrix4<float>&, const Vector2<float>&);

        private:
            Widget& getReferenceWidget() const;
            TextureParam::Anisotropy getTextureAnisotropy() const;

            static constexpr uint32_t NORMAL_MATRIX_UNIFORM_BINDING = 0;
            static constexpr uint32_t PVM_MATRIX_UNIFORM_BINDING = 1;
            static constexpr uint32_t COLOR_PARAMS_UNIFORM_BINDING = 2;
            static constexpr uint32_t CAMERA_INFO_UNIFORM_BINDING = 3;
            static constexpr uint32_t TEX_UNIFORM_BINDING = 4;

            bool isInitialized;

            const WidgetSetDisplayer& widgetSetDisplayer;
            std::vector<Widget*> instanceWidgets;
            std::size_t instanceId;

            const UIRenderer& uiRenderer;

            std::vector<Point2<float>> coordinates;
            struct ColorParams {
                alignas(4) float alphaFactor;
                alignas(4) float gammaFactor;
            } colorParams;
            struct CameraInfo {
                alignas(8) Vector2<float> jitterInPixel;
            } cameraInfo;
            mutable std::vector<Matrix4<float>> instanceModelMatrices;

            std::unique_ptr<GenericRenderer> renderer;
            std::chrono::steady_clock::time_point lastRenderingTime;
    };

}