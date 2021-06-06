#include <string>

#include <scene/SceneManager.h>
#include <graphic/render/target/RenderTarget.h>

#define START_FPS 1000.0f //high number of FPS to avoid pass through the ground at startup
#define REFRESH_RATE_FPS 0.35f

namespace urchin {

    SceneManager::SceneManager(const std::vector<std::string>& windowRequiredExtensions, const std::unique_ptr<SurfaceCreator>& surfaceCreator, std::unique_ptr<FramebufferSizeRetriever> framebufferSizeRetriever) :
            sceneWidth(500),
            sceneHeight(500),
            screenRenderTarget(std::make_shared<ScreenRender>("screen", RenderTarget::NO_DEPTH_ATTACHMENT)),
            activeRenderer3d(nullptr),
            activeUiRenderers(nullptr),
            previousFps(),
            fps(START_FPS),
            fpsForDisplay(START_FPS) {
        //initialize
        SignalHandler::instance()->initialize();
        GraphicService::instance()->initialize(windowRequiredExtensions, surfaceCreator, std::move(framebufferSizeRetriever));
        screenRenderTarget->initialize();

        //initialize fps
        previousFps.fill(START_FPS);
        indexFps = (unsigned int)previousFps.size();
        previousTime = std::chrono::high_resolution_clock::now();
    }

    SceneManager::~SceneManager() {
        screenRenderTarget->cleanup();

        for (auto& renderer3d : renderers3d) {
            delete renderer3d;
        }
        for (auto& uiRenderer : uiRenderers) {
            delete uiRenderer;
        }

        Profiler::graphic()->log();
    }

    void SceneManager::updateVerticalSync(bool verticalSyncEnabled) {
        screenRenderTarget->updateVerticalSync(verticalSyncEnabled);
    }

    void SceneManager::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        if (sceneWidth != 0 && sceneHeight != 0) {
            //scene properties
            this->sceneWidth = sceneWidth;
            this->sceneHeight = sceneHeight;

            //renderer
            screenRenderTarget->onResize();
            for(auto* activeRenderer : std::initializer_list<Renderer*>{activeRenderer3d, activeUiRenderers}) {
                if(activeRenderer) {
                    activeRenderer->onResize(sceneWidth, sceneHeight);
                }
            }
        }
    }

    unsigned int SceneManager::getSceneWidth() const {
        return sceneWidth;
    }

    unsigned int SceneManager::getSceneHeight() const {
        return sceneHeight;
    }

    float SceneManager::getFps() const {
        return (fps == 0.0f ? START_FPS : fps);
    }

    unsigned int SceneManager::getFpsForDisplay() {
        static float timeElapse = 0.0f;
        timeElapse += getDeltaTime();

        if (timeElapse > REFRESH_RATE_FPS) { //refresh fps every REFRESH_RATE_FPS_MS
            fpsForDisplay = MathFunction::roundToUInt(fps);
            timeElapse = 0.0f;
        }

        return fpsForDisplay;
    }

    float SceneManager::getDeltaTime() const {
        return 1.0f / getFps();
    }

    void SceneManager::computeFps() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto timeInterval = (float)std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime).count();

        previousFps[indexFps % 3] = 1000000.0f / timeInterval;
        fps = (previousFps.at(indexFps % 3) * 9 + previousFps.at((indexFps - 1) % 3) * 3 + previousFps.at((indexFps - 2) % 3)) / 13.0f;

        indexFps++;
        previousTime = currentTime;
    }

    Renderer3d* SceneManager::newRenderer3d(bool enable) {
        auto* renderer3d = new Renderer3d(screenRenderTarget);
        renderers3d.push_back(renderer3d);

        if (enable) {
            enableRenderer3d(renderer3d);
        }
        return renderer3d;
    }

    void SceneManager::enableRenderer3d(Renderer3d* renderer3d) {
        if (activeRenderer3d && activeRenderer3d != renderer3d) {
            activeRenderer3d->onDisable();
        }

        activeRenderer3d = renderer3d;
        if (renderer3d) {
            renderer3d->onResize(sceneWidth, sceneHeight);
        }
    }

    void SceneManager::removeRenderer3d(Renderer3d* renderer3d) {
        if (activeRenderer3d == renderer3d) {
            activeRenderer3d = nullptr;
        }

        renderers3d.erase(std::remove(renderers3d.begin(), renderers3d.end(), renderer3d), renderers3d.end());
        delete renderer3d;
    }

    Renderer3d* SceneManager::getActiveRenderer3d() const {
        return activeRenderer3d;
    }

    UIRenderer* SceneManager::newUIRenderer(bool enable) {
        auto* uiRenderer = new UIRenderer(screenRenderTarget);
        uiRenderers.push_back(uiRenderer);

        if (enable) {
            enableUIRenderer(uiRenderer);
        }
        return uiRenderer;
    }

    void SceneManager::enableUIRenderer(UIRenderer* uiRenderer) {
        if (activeUiRenderers && activeUiRenderers != uiRenderer) {
            activeUiRenderers->onDisable();
        }

        activeUiRenderers = uiRenderer;
        if (uiRenderer) {
            uiRenderer->onResize(sceneWidth, sceneHeight);
        }
    }

    void SceneManager::removeUIRenderer(UIRenderer* uiRenderer) {
        if (activeUiRenderers == uiRenderer) {
            activeUiRenderers = nullptr;
        }

        uiRenderers.erase(std::remove(uiRenderers.begin(), uiRenderers.end(), uiRenderer), uiRenderers.end());
        delete uiRenderer;
    }

    UIRenderer* SceneManager::getActiveUIRenderer() const {
        return dynamic_cast<UIRenderer*>(activeUiRenderers);
    }

    bool SceneManager::onKeyPress(unsigned int key) {
        for(auto* activeRenderer : std::initializer_list<Renderer*>{activeUiRenderers, activeRenderer3d}) {
            if (activeRenderer && !activeRenderer->onKeyPress(key)) {
                return false;
            }
        }
        return true;
    }

    bool SceneManager::onKeyRelease(unsigned int key) {
        for(auto* activeRenderer : std::initializer_list<Renderer*>{activeUiRenderers, activeRenderer3d}) {
            if (activeRenderer && !activeRenderer->onKeyRelease(key)) {
                return false;
            }
        }
        return true;
    }

    bool SceneManager::onChar(unsigned int character) {
        for(auto* activeRenderer : std::initializer_list<Renderer*>{activeUiRenderers, activeRenderer3d}) {
            if (activeRenderer && !activeRenderer->onChar(character)) {
                return false;
            }
        }
        return true;
    }

    bool SceneManager::onMouseMove(int mouseX, int mouseY) {
        for(auto* activeRenderer : std::initializer_list<Renderer*>{activeUiRenderers, activeRenderer3d}) {
            if (activeRenderer && !activeRenderer->onMouseMove(mouseX, mouseY)) {
                return false;
            }
        }
        return true;
    }

    void SceneManager::display() {
        ScopeProfiler sp(Profiler::graphic(), "sceneMgrDisplay");

        //fps
        computeFps();
        float dt = getDeltaTime();

        //renderer
        screenRenderTarget->disableAllRenderers();
        for(auto* activeRenderer : std::initializer_list<Renderer*>{activeRenderer3d, activeUiRenderers}) {
            if(activeRenderer) {
                activeRenderer->prepareRendering(dt);
            }
        }
        screenRenderTarget->render();
    }

}
