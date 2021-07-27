#pragma once

#include <set>
#include <UrchinCommon.h>

#include <graphic/render/shader/model/Shader.h>
#include <graphic/render/target/RenderTarget.h>
#include <scene/Renderer.h>
#include <scene/ui/widget/Widget.h>
#include <texture/render/TextureRenderer.h>

namespace urchin {

    class UIRenderer : public Renderer, public Observer {
        public:
            explicit UIRenderer(RenderTarget&, I18nService&);

            void onResize(unsigned int, unsigned int) override;
            void notify(Observable*, int) override;

            bool onKeyPress(unsigned int) override;
            bool onKeyRelease(unsigned int) override;
            bool onChar(char32_t) override;
            bool onMouseMove(double, double) override;
            bool onScroll(double) override;
            void onDisable() override;

            void addWidget(const std::shared_ptr<Widget>&);
            void removeWidget(Widget&);

            void prepareRendering(float) override;

        private:
            RenderTarget& renderTarget;
            I18nService& i18nService;
            std::vector<std::shared_ptr<Widget>> widgets;

            std::unique_ptr<Shader> uiShader;

            std::unique_ptr<TextureRenderer> debugFont;
    };

}
