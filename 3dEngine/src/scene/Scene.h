#pragma once

#include <chrono>
#include <vector>
#include <array>
#include <UrchinCommon.h>

#include <scene/renderer3d/Renderer3d.h>
#include <scene/ui/UIRenderer.h>
#include <api/setup/GraphicService.h>
#include <api/render/target/ScreenRender.h>

namespace urchin {

    class Scene {
        public:
            Scene(const std::vector<std::string>&, std::unique_ptr<SurfaceCreator>, std::unique_ptr<FramebufferSizeRetriever>);
            ~Scene();

            //scene properties
            void updateVerticalSync(bool);
            void onResize();
            unsigned int getSceneWidth() const;
            unsigned int getSceneHeight() const;
            I18nService& getI18nService();

            //fps
            float getFps() const;
            unsigned int getFpsForDisplay();
            float getDeltaTime() const;
            void setFpsLimit(int);

            //renderer
            Renderer3d& newRenderer3d(std::shared_ptr<Camera>, const VisualConfig&, bool);
            void enableRenderer3d(Renderer3d*);
            void removeRenderer3d(const Renderer3d*);
            Renderer3d* getActiveRenderer3d() const;

            UIRenderer& newUIRenderer(bool);
            void enableUIRenderer(UIRenderer*);
            void removeUIRenderer(const UIRenderer*);
            UIRenderer* getActiveUIRenderer() const;

            void takeScreenShot(const std::string&, unsigned int = 0, unsigned int = 0) const;

            //events
            bool onKeyPress(unsigned int);
            bool onKeyRelease(unsigned int);
            bool onChar(char32_t);
            bool onMouseMove(double, double);
            bool onScroll(double);

            //render
            void display();

        private:
            void handleFpsLimiter();
            void computeFps();
            void resetFps();

            static constexpr float STARTUP_FPS = 1000.0f; //high number of FPS to avoid pass through the ground at startup
            static constexpr float FPS_REFRESH_TIME_IN_MS = 3.0f;
            static constexpr float DISPLAY_FPS_REFRESH_TIME_IN_MS = 400.0f;
            static constexpr std::chrono::steady_clock::time_point MIN_TIME_POINT = std::chrono::steady_clock::time_point::min();

            //scene properties
            std::unique_ptr<FramebufferSizeRetriever> framebufferSizeRetriever;
            I18nService i18nService;
            unsigned int sceneWidth;
            unsigned int sceneHeight;

            //fps
            std::chrono::steady_clock::time_point fpsLimitPreviousTime;
            std::chrono::steady_clock::time_point fpsPreviousTime;
            float fps;
            unsigned int fpsForDisplay;
            int fpsLimit;

            //renderer
            std::uint64_t frameIndex;
            ScreenRender screenRenderTarget;
            std::vector<std::unique_ptr<Renderer3d>> renderers3d;
            Renderer3d* activeRenderer3d;
            std::vector<std::unique_ptr<UIRenderer>> uiRenderers;
            UIRenderer* activeUiRenderers;
    };

}
