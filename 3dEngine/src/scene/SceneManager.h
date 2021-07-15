#pragma once

#include <chrono>
#include <vector>
#include <array>
#include <UrchinCommon.h>

#include <scene/renderer3d/Renderer3d.h>
#include <scene/ui/UIRenderer.h>
#include <graphic/setup/GraphicService.h>
#include <graphic/render/target/ScreenRender.h>

namespace urchin {

    class SceneManager {
        public:
            SceneManager(const std::vector<std::string>&, const std::unique_ptr<SurfaceCreator>&, std::unique_ptr<FramebufferSizeRetriever>);
            ~SceneManager();

            //scene properties
            void updateVerticalSync(bool);
            void onResize();
            unsigned int getSceneWidth() const;
            unsigned int getSceneHeight() const;
            I18nService& getI18nService() const;

            //fps
            float getFps() const;
            unsigned int getFpsForDisplay();
            float getDeltaTime() const;

            //renderer
            Renderer3d& newRenderer3d(bool);
            void enableRenderer3d(Renderer3d*);
            void removeRenderer3d(Renderer3d*);
            Renderer3d* getActiveRenderer3d() const;

            UIRenderer& newUIRenderer(bool);
            void enableUIRenderer(UIRenderer*);
            void removeUIRenderer(UIRenderer*);
            UIRenderer* getActiveUIRenderer() const;

            //events
            bool onKeyPress(unsigned int);
            bool onKeyRelease(unsigned int);
            bool onChar(char32_t);
            bool onMouseMove(double, double);

            //scene
            void display();

        private:
            void computeFps();

            static float STARTUP_FPS;
            static float FPS_REFRESH_TIME_IN_MS;
            static float DISPLAY_FPS_REFRESH_TIME_IN_MS;

            //scene properties
            std::unique_ptr<FramebufferSizeRetriever> framebufferSizeRetriever;
            unsigned int sceneWidth, sceneHeight;
            std::unique_ptr<I18nService> i18nService;

            //fps
            std::chrono::steady_clock::time_point previousTime;
            float fps;
            unsigned int fpsForDisplay;

            //renderer
            std::unique_ptr<ScreenRender> screenRenderTarget;
            std::vector<std::unique_ptr<Renderer3d>> renderers3d;
            Renderer3d* activeRenderer3d;
            std::vector<std::unique_ptr<UIRenderer>> uiRenderers;
            UIRenderer* activeUiRenderers;
    };

}
