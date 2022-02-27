#pragma once

#include <set>
#include <UrchinCommon.h>

#include <api/render/shader/model/Shader.h>
#include <api/render/target/RenderTarget.h>
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

        std::unique_ptr<Plane<float>> uiPlane;
        Point3<float> uiPosition;

        float maxInteractiveDistance = 4.0f;
        UI3dPointerType pointerType = MOUSE;
    };

    class UIRenderer : public Renderer, public Observer {
        public:
            UIRenderer(RenderTarget&, I18nService&);
            ~UIRenderer() override;

            //3d specific
            void setupUi3d(Camera*, const Transform<float>&, const Point2<int>&, const Point2<float>&, float);
            void onCameraProjectionUpdate(Camera&);
            void setMaximumInteractiveDistance(float) const;
            void setPointerType(UI3dPointerType) const;

            void onResize(unsigned int, unsigned int) override;
            void notify(Observable*, int) override;

            bool onKeyPress(unsigned int) override;
            bool onKeyRelease(unsigned int) override;
            bool onChar(char32_t) override;
            bool onMouseMove(double, double) override;
            bool onScroll(double) override;
            void onDisable() override;

            RenderTarget& getRenderTarget() const;
            I18nService& getI18nService() const;
            const Point2<int>& getUiResolution() const;
            Shader& getShader() const;
            UI3dData* getUi3dData() const;
            bool canInteractWithUi() const;

            void addWidget(const std::shared_ptr<Widget>&);
            void removeWidget(const Widget&);
            void removeAllWidgets();
            const std::vector<std::shared_ptr<Widget>>& getWidgets() const;

            void prepareRendering(float, unsigned int&) override;
            void prepareRendering(float, unsigned int&, const Matrix4<float>&) const;

        private:
            bool onCursorMove(double, double);
            bool adjustMouseCoordinates(const Point2<double>&, Point2<int>&) const;

            RenderTarget& renderTarget;
            I18nService& i18nService;
            Point2<int> uiResolution;
            std::unique_ptr<Shader> uiShader;

            double rawMouseX;
            double rawMouseY;
            bool bCanInteractWithUi;

            std::unique_ptr<UI3dData> ui3dData;
            std::vector<std::shared_ptr<Widget>> widgets;

            std::unique_ptr<TextureRenderer> debugFont;
    };

}
