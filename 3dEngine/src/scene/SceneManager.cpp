#include <string>

#include <scene/SceneManager.h>
#include <graphic/render/target/RenderTarget.h>

namespace urchin {

    //static
    float SceneManager::STARTUP_FPS = 1000.0f; //high number of FPS to avoid pass through the ground at startup
    float SceneManager::FPS_REFRESH_TIME_IN_MS = 4.0f;
    float SceneManager::DISPLAY_FPS_REFRESH_TIME_IN_MS = 400.0f;

    SceneManager::SceneManager(const std::vector<std::string>& windowRequiredExtensions, const std::unique_ptr<SurfaceCreator>& surfaceCreator, std::unique_ptr<FramebufferSizeRetriever> framebufferSizeRetriever) :
            framebufferSizeRetriever(std::move(framebufferSizeRetriever)),
            sceneWidth(0),
            sceneHeight(0),
            i18nService(std::make_unique<I18nService>()),
            previousTime(),
            fps(STARTUP_FPS),
            fpsForDisplay((unsigned int)STARTUP_FPS),
            screenRenderTarget(std::make_shared<ScreenRender>("screen", RenderTarget::NO_DEPTH_ATTACHMENT)),
            activeRenderer3d(nullptr),
            activeUiRenderers(nullptr) {
        //scene properties
        this->framebufferSizeRetriever->getFramebufferSizeInPixel(sceneWidth, sceneHeight);

        //initialize
        SignalHandler::instance()->initialize();
        GraphicService::instance()->initialize(windowRequiredExtensions, surfaceCreator, this->framebufferSizeRetriever.get());
        screenRenderTarget->initialize();
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

    void SceneManager::onResize() {
        unsigned int newSceneWidth, newSceneHeight;
        framebufferSizeRetriever->getFramebufferSizeInPixel(newSceneWidth, newSceneHeight);

        if((newSceneWidth > 1 && newSceneHeight > 1) && (sceneWidth != newSceneWidth || sceneHeight != newSceneHeight)) {
            Logger::instance()->logInfo("Resize scene to " + std::to_string(newSceneWidth) + "x" + std::to_string(newSceneHeight));

            //scene properties
            this->sceneWidth = newSceneWidth;
            this->sceneHeight = newSceneHeight;

            //renderer
            screenRenderTarget->onResize();
            for (auto* renderer3d : renderers3d) {
                renderer3d->onResize(sceneWidth, sceneHeight);
            }
            for (auto* uiRenderer : uiRenderers) {
                uiRenderer->onResize(sceneWidth, sceneHeight);
            }
        }
    }

    unsigned int SceneManager::getSceneWidth() const {
        return sceneWidth;
    }

    unsigned int SceneManager::getSceneHeight() const {
        return sceneHeight;
    }

    const std::unique_ptr<I18nService>& SceneManager::getI18nService() const {
        return i18nService;
    }

    float SceneManager::getFps() const {
        return fps;
    }

    unsigned int SceneManager::getFpsForDisplay() {
        static float timeElapseInSec = 0.0f;
        timeElapseInSec += getDeltaTime();

        if (timeElapseInSec * 1000.0f > DISPLAY_FPS_REFRESH_TIME_IN_MS) {
            fpsForDisplay = MathFunction::roundToUInt(fps);
            timeElapseInSec = 0.0f;
        }

        return fpsForDisplay;
    }

    float SceneManager::getDeltaTime() const {
        return 1.0f / getFps();
    }

    void SceneManager::computeFps() {
        if (previousTime.time_since_epoch().count() == 0) {
            previousTime = std::chrono::steady_clock::now();
        }
        auto currentTime = std::chrono::steady_clock::now();
        auto deltaTimeInUs = (double)std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime).count();

        static int frameCount = 0;
        frameCount++;
        if (deltaTimeInUs / 1000.0 > FPS_REFRESH_TIME_IN_MS) {
            fps = (float)(1000000.0 / deltaTimeInUs) * (float)frameCount;
            previousTime = currentTime;
            frameCount = 0;
        }
    }

    Renderer3d* SceneManager::newRenderer3d(bool enable) {
        auto* renderer3d = new Renderer3d(screenRenderTarget);
        renderer3d->onResize(sceneWidth, sceneHeight);
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
        auto* uiRenderer = new UIRenderer(screenRenderTarget, i18nService);
        uiRenderer->onResize(sceneWidth, sceneHeight);
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

    bool SceneManager::onChar(char32_t unicodeCharacter) {
        for(auto* activeRenderer : std::initializer_list<Renderer*>{activeUiRenderers, activeRenderer3d}) {
            if (activeRenderer && !activeRenderer->onChar(unicodeCharacter)) {
                return false;
            }
        }
        return true;
    }

    bool SceneManager::onMouseMove(double mouseX, double mouseY) {
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
