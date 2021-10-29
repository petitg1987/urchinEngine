#pragma once

#include <set>
#include <UrchinCommon.h>

#include <api/render/shader/model/Shader.h>
#include <api/render/target/RenderTarget.h>
#include <scene/Renderer.h>
#include <scene/ui/widget/Widget.h>
#include <texture/render/TextureRenderer.h>

namespace urchin {

    struct UI3dData {
        Matrix4<float> cameraProjectionMatrix;
        Matrix4<float> modelMatrix;
        Matrix4<float> normalMatrix;
    };

    class UIRenderer : public Renderer, public Observer {
        public:
            explicit UIRenderer(RenderTarget&, I18nService&);

            //3d specific
            void setupUi3d(const Matrix4<float>&, const Transform<float>&, const Point2<unsigned int>&, const Point2<float>&, float);
            void onCameraProjectionUpdate(const Matrix4<float>&);

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
            const Point2<unsigned int>& getSceneSize() const;
            Shader& getShader() const;
            UI3dData* getUi3dData() const;

            void addWidget(const std::shared_ptr<Widget>&);
            void removeWidget(Widget&);
            void removeAllWidgets();
            const std::vector<std::shared_ptr<Widget>>& getWidgets() const;

            void prepareRendering(float, unsigned int&) override;
            void prepareRendering(float, unsigned int&, const Matrix4<float>&);

        private:
            RenderTarget& renderTarget;
            I18nService& i18nService;
            Point2<unsigned int> sceneSize;
            std::unique_ptr<Shader> uiShader;
            std::unique_ptr<UI3dData> ui3dData;

            std::vector<std::shared_ptr<Widget>> widgets;

            std::unique_ptr<TextureRenderer> debugFont;
    };

}
