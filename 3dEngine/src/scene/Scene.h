#pragma once

#include <chrono>
#include <vector>

#include "scene/InputDeviceKey.h"
#include "scene/renderer3d/Renderer3d.h"
#include "scene/ui/UIRenderer.h"
#include "graphics/api/GraphicsApi.h"

namespace urchin {

    class Scene {
        public:
            Scene(const std::vector<std::string>&, std::unique_ptr<SurfaceCreator>, std::unique_ptr<FramebufferSizeRetriever>, bool);
            ~Scene();

            //scene properties
            void updateVerticalSync(bool);
            void onResize();
            unsigned int getSceneWidth() const;
            unsigned int getSceneHeight() const;
            I18nService& getI18nService();

            //time
            void setFpsLimit(int);
            float getDeltaTimeInSec() const;
            const PerfMetrics& getPerfMetrics() const;

            //renderer
            Renderer3d& newRenderer3d(std::shared_ptr<Camera>, const VisualConfig&, bool);
            void enableRenderer3d(Renderer3d*);
            void removeRenderer3d(const Renderer3d*);
            Renderer3d* getActiveRenderer3d() const;

            UIRenderer& newUIRenderer(bool);
            void enableUIRenderer(UIRenderer*);
            void removeUIRenderer(const UIRenderer*);
            UIRenderer* getActiveUIRenderer() const;

            void updateGammaFactor(float);
            void takeScreenShot(const std::string&, unsigned int = 0, unsigned int = 0) const;

            //events
            bool onKeyPress(InputDeviceKey);
            bool onKeyRelease(InputDeviceKey);
            bool onChar(char32_t);
            bool onMouseMove(double, double, double, double);
            bool onScroll(double);

            //render
            void display();

        private:
            void handleFpsLimiter();
            void computeDeltaTime();
            void resetDeltaTime();

            static constexpr float STARTUP_TIME_STEP = 0.001f; //low time step to avoid pass through the ground at startup
            static constexpr float REFRESH_TIME_IN_MS = 2.0f;
            static constexpr std::chrono::steady_clock::time_point MIN_TIME_POINT = std::chrono::steady_clock::time_point::min();

            //scene properties
            std::unique_ptr<FramebufferSizeRetriever> framebufferSizeRetriever;
            I18nService i18nService;
            unsigned int sceneWidth;
            unsigned int sceneHeight;

            //time
            std::chrono::steady_clock::time_point fpsLimitPreviousTime;
            std::chrono::steady_clock::time_point previousCurrentTime;
            float deltaTimeInSec;
            int fpsLimit;
            PerfMetrics perfMetrics;

            //renderer
            std::unique_ptr<GraphicsApiService> graphicsApiService;
            uint32_t frameCount;
            ScreenRender screenRenderTarget;
            std::vector<std::unique_ptr<Renderer3d>> renderers3d;
            Renderer3d* activeRenderer3d;
            std::vector<std::unique_ptr<UIRenderer>> uiRenderers;
            UIRenderer* activeUiRenderer;
            float gammaFactor;
    };

}
