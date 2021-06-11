#pragma once

#include <set>
#include <UrchinCommon.h>

#include <graphic/render/shader/model/Shader.h>
#include <graphic/render/target/RenderTarget.h>
#include <scene/Renderer.h>
#include <scene/UI/widget/Widget.h>
#include <texture/render/TextureRenderer.h>

namespace urchin {

    class UIRenderer : public Renderer, public Observer {
        public:
            explicit UIRenderer(std::shared_ptr<RenderTarget>);
            ~UIRenderer() override;

            void onResize(unsigned int, unsigned int) override;
            void notify(Observable*, int) override;

            bool onKeyPress(unsigned int) override;
            bool onKeyRelease(unsigned int) override;
            bool onChar(char32_t) override;
            bool onMouseMove(int, int) override;
            void onDisable() override;

            void addWidget(Widget*);
            void removeWidget(Widget*);

            void prepareRendering(float) override;

        private:
            std::shared_ptr<RenderTarget> renderTarget;
            std::vector<Widget*> widgets;

            Matrix4<float> mProjection;
            std::shared_ptr<Shader> uiShader;

            std::unique_ptr<I18nService> i18nService;

            std::unique_ptr<TextureRenderer> debugFont;
    };

}
