#pragma once

#include <UrchinCommon.h>

#include <graphics/api/GraphicsApi.h>
#include <scene/Renderer.h>
#include <scene/ui/widget/Widget.h>
#include <scene/renderer3d/camera/CameraSpaceService.h>
#include <texture/render/TextureRenderer.h>

namespace urchin {

    enum UI3dPointerType {
        MOUSE,
        SCREEN_CENTER
    };

    struct UI3dData {
        Camera* camera = nullptr;
        Matrix4<float> modelMatrix;
        Matrix4<float> normalMatrix;

        Plane<float> uiPlane;
        Point3<float> uiPosition;
        Sphere<float> uiSphereBounding;

        float maxInteractiveDistance = 4.0f;
        UI3dPointerType pointerType = MOUSE;
    };

    class WidgetSetDisplayer;

    class UIRenderer final : public Renderer, public Observer {
        public:
            UIRenderer(float, RenderTarget&, I18nService&);
            ~UIRenderer() override;

            //3d specific
            void setupUi3d(Camera*, const Transform<float>&, const Point2<int>&, const Point2<float>&, float);
            void setupClipboard(std::unique_ptr<Clipboard>);
            void onCameraProjectionUpdate(Camera&);
            void setMaximumInteractiveDistance(float) const;
            void setPointerType(UI3dPointerType) const;

            void onResize(unsigned int, unsigned int) override;
            void notify(Observable*, int) override;

            bool onKeyPress(InputDeviceKey) override;
            bool onKeyRelease(InputDeviceKey) override;
            bool onChar(char32_t) override;
            bool onMouseMove(double, double, double, double) override;
            bool onCursorMove();
            bool onScroll(double) override;
            void onDisable() override;

            RenderTarget& getRenderTarget() const;
            I18nService& getI18nService() const;
            Clipboard& getClipboard() const;
            const Point2<int>& getUiResolution() const;
            Shader& getShader() const;
            UI3dData* getUi3dData() const;
            bool canInteractWithUi() const;
            WidgetSetDisplayer& getWidgetSetDisplayer() const;

            void applyUpdatedGammaFactor() override;

            void addWidget(const std::shared_ptr<Widget>&);
            void removeWidget(const Widget&);
            void removeAllWidgets();
            bool isWidgetExist(const Widget&) const;
            const std::vector<std::shared_ptr<Widget>>& getWidgets() const;

            void prepareRendering(uint32_t, float, unsigned int&) override;
            void prepareRendering(uint32_t, float, unsigned int&, const Matrix4<float>&, const Vector2<float>&) const;

        private:
            bool adjustMouseCoordinates(Point2<int>&) const;
            void prepareWidgets(float, const std::vector<std::shared_ptr<Widget>>&) const;

            RenderTarget& renderTarget;
            I18nService& i18nService;
            std::unique_ptr<Clipboard> clipboard;
            Point2<int> uiResolution;
            std::unique_ptr<Shader> uiShader;

            double rawMouseX;
            double rawMouseY;
            bool bCanInteractWithUi;

            std::unique_ptr<UI3dData> ui3dData;
            std::vector<std::shared_ptr<Widget>> widgets;

            std::unique_ptr<WidgetSetDisplayer> widgetSetDisplayer;

            mutable EverGrowQueue<Widget*> widgetsQueue;
            mutable EverGrowQueue<Widget*> postponedWidgetsQueue;
            mutable std::vector<Widget*> widgetsToRender;

            std::unique_ptr<TextureRenderer> debugFont;
    };

}
