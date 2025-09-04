#include <string>

#include "scene/Scene.h"
#include "resources/ResourceContainer.h"

namespace urchin {

    //debug parameters
    constexpr auto DEBUG_PROFILE_FRAME_BY_FRAME = False();

    Scene::Scene(const std::vector<std::string>& windowRequiredExtensions, std::unique_ptr<SurfaceCreator> surfaceCreator, std::unique_ptr<FramebufferSizeRetriever> framebufferSizeRetriever, bool verticalSyncEnabled) :
            framebufferSizeRetriever(std::move(framebufferSizeRetriever)),
            i18nService(I18nService()),
            sceneWidth(0),
            sceneHeight(0),
            fps(0),
            fpsForDisplay(0),
            fpsLimit(-1),
            frameCount(0),
            screenRenderTarget(ScreenRender("screen", false, RenderTarget::NO_DEPTH_ATTACHMENT)),
            activeRenderer3d(nullptr),
            activeUiRenderer(nullptr),
            gammaFactor(1.0f) {
        //scene properties
        this->framebufferSizeRetriever->getFramebufferSizeInPixel(sceneWidth, sceneHeight);

        //fps
        resetFps();

        //initialize
        SignalHandler::instance().initialize();
        graphicsApiService = std::make_unique<GraphicsApiService>(windowRequiredExtensions, std::move(surfaceCreator), *this->framebufferSizeRetriever);
        screenRenderTarget.updateVerticalSync(verticalSyncEnabled);
        screenRenderTarget.initialize();
    }

    Scene::~Scene() {
        activeRenderer3d = nullptr;
        renderers3d.clear();
        activeUiRenderer = nullptr;
        uiRenderers.clear();

        ResourceContainer::instance().cleanResources();
        if (!DEBUG_PROFILE_FRAME_BY_FRAME) {
            Profiler::graphic().log();
        }
    }

    void Scene::updateVerticalSync(bool verticalSyncEnabled) {
        screenRenderTarget.updateVerticalSync(verticalSyncEnabled);
    }

    void Scene::onResize() {
        unsigned int newSceneWidth = 0;
        unsigned int newSceneHeight = 0;
        framebufferSizeRetriever->getFramebufferSizeInPixel(newSceneWidth, newSceneHeight);

        if ((newSceneWidth != 0 && newSceneHeight != 0) && (sceneWidth != newSceneWidth || sceneHeight != newSceneHeight)) {
            Logger::instance().logInfo("Resizing the scene to " + std::to_string(newSceneWidth) + "x" + std::to_string(newSceneHeight));

            //scene properties
            this->sceneWidth = newSceneWidth;
            this->sceneHeight = newSceneHeight;

            //renderer
            screenRenderTarget.onResize();
            for (const auto& renderer3d : renderers3d) {
                renderer3d->onResize(sceneWidth, sceneHeight);
            }
            for (const auto& uiRenderer : uiRenderers) {
                uiRenderer->onResize(sceneWidth, sceneHeight);
            }
        }
    }

    unsigned int Scene::getSceneWidth() const {
        return sceneWidth;
    }

    unsigned int Scene::getSceneHeight() const {
        return sceneHeight;
    }

    I18nService& Scene::getI18nService() {
        return i18nService;
    }

    float Scene::getFps() const {
        return fps;
    }

    unsigned int Scene::getFpsForDisplay() {
        static float timeElapseInSec = 0.0f;
        timeElapseInSec += getDeltaTime();

        if (timeElapseInSec * 1000.0f > DISPLAY_FPS_REFRESH_TIME_IN_MS) {
            fpsForDisplay = MathFunction::roundToUInt(fps);
            timeElapseInSec = 0.0f;
        }

        return fpsForDisplay;
    }

    float Scene::getDeltaTime() const {
        return 1.0f / fps;
    }

    void Scene::setFpsLimit(int fpsLimit) {
        this->fpsLimit = fpsLimit;
        resetFps();
    }

    const PerfMetrics& Scene::getPerfMetrics() const {
        return perfMetrics;
    }

    void Scene::handleFpsLimiter() {
        ScopeProfiler sp(Profiler::graphic(), "fpsLimit");
        if (fpsLimit > 0 && !screenRenderTarget.isVerticalSyncEnabled()) {
            if (fpsLimitPreviousTime == MIN_TIME_POINT) [[unlikely]] {
                fpsLimitPreviousTime = std::chrono::steady_clock::now();
            } else {
                auto currentTime = std::chrono::steady_clock::now();
                auto deltaTimeInUs = (double)std::chrono::duration_cast<std::chrono::microseconds>(currentTime - fpsLimitPreviousTime).count();

                double expectedDeltaInUs = (1000.0 * 1000.0) / (double)fpsLimit;
                if (deltaTimeInUs < expectedDeltaInUs) {
                    SleepUtil::sleepUs(std::lround(expectedDeltaInUs - deltaTimeInUs));
                    fpsLimitPreviousTime = std::chrono::steady_clock::now();
                } else {
                    fpsLimitPreviousTime = currentTime;
                }
            }
        }
    }

    void Scene::computeFps() {
        if (fpsPreviousTime == MIN_TIME_POINT) [[unlikely]] {
            fpsPreviousTime = std::chrono::steady_clock::now();
        } else {
            auto currentTime = std::chrono::steady_clock::now();
            auto deltaTimeInUs = (double) std::chrono::duration_cast<std::chrono::microseconds>(currentTime - fpsPreviousTime).count();

            static int frameCount = 0;
            frameCount++;
            if (deltaTimeInUs / 1000.0 > FPS_REFRESH_TIME_IN_MS) {
                fps = (float) (1000000.0 / deltaTimeInUs) * (float) frameCount;
                fpsPreviousTime = currentTime;
                frameCount = 0;

                if (getActiveRenderer3d()) {
                    float frameTimeInSec = (float)(deltaTimeInUs / 1000000.0);
                    perfMetrics.registerDt(frameTimeInSec); //Can contain bad metrics: first call on WidgetSetDisplayer::updateWidgets, camera rotation loading new 3d models, etc
                }
            }
        }
    }

    void Scene::resetFps() {
        fpsLimitPreviousTime = MIN_TIME_POINT;
        fpsPreviousTime = MIN_TIME_POINT;
        fps = STARTUP_FPS;
        fpsForDisplay = (unsigned int)STARTUP_FPS;
    }

    Renderer3d& Scene::newRenderer3d(std::shared_ptr<Camera> camera, const VisualConfig& visualConfig, bool enable) {
        auto renderer3d = std::make_unique<Renderer3d>(gammaFactor, screenRenderTarget, std::move(camera), visualConfig, i18nService);

        renderers3d.push_back(std::move(renderer3d));
        if (enable) {
            enableRenderer3d(renderers3d.back().get());
        }
        return *renderers3d.back();
    }

    void Scene::enableRenderer3d(Renderer3d* renderer3d) {
        if (activeRenderer3d && activeRenderer3d != renderer3d) {
            activeRenderer3d->onDisable();
        }
        resetFps();
        activeRenderer3d = renderer3d;
    }

    void Scene::removeRenderer3d(const Renderer3d* renderer3d) {
        if (activeRenderer3d == renderer3d) {
            activeRenderer3d = nullptr;
        }
        std::erase_if(renderers3d, [&](auto& p){return renderer3d == p.get();});
    }

    Renderer3d* Scene::getActiveRenderer3d() const {
        return activeRenderer3d;
    }

    UIRenderer& Scene::newUIRenderer(bool enable) {
        auto uiRenderer = std::make_unique<UIRenderer>(gammaFactor, screenRenderTarget, i18nService);

        uiRenderers.push_back(std::move(uiRenderer));
        if (enable) {
            enableUIRenderer(uiRenderers.back().get());
        }
        return *uiRenderers.back();
    }

    void Scene::enableUIRenderer(UIRenderer* uiRenderer) {
        if (activeUiRenderer && activeUiRenderer != uiRenderer) {
            activeUiRenderer->onDisable();
        }
        activeUiRenderer = uiRenderer;
    }

    void Scene::removeUIRenderer(const UIRenderer* uiRenderer) {
        if (activeUiRenderer == uiRenderer) {
            activeUiRenderer = nullptr;
        }
        std::erase_if(uiRenderers, [&](auto& p){return uiRenderer == p.get();});
    }

    UIRenderer* Scene::getActiveUIRenderer() const {
        return activeUiRenderer;
    }

    void Scene::updateGammaFactor(float gammaFactor) {
        this->gammaFactor = gammaFactor;

        for (const auto& renderer3d : renderers3d) {
            renderer3d->onGammaFactorUpdate(gammaFactor);
        }
        for (const auto& uiRenderer : uiRenderers) {
            uiRenderer->onGammaFactorUpdate(gammaFactor);
        }
    }

    void Scene::takeScreenShot(const std::string& filename, unsigned int width, unsigned int height) const {
        screenRenderTarget.takeScreenshot(filename, width, height);
    }

    bool Scene::onKeyPress(InputDeviceKey key) {
        for (auto* activeRenderer : std::initializer_list<Renderer*>{activeUiRenderer, activeRenderer3d}) {
            if (activeRenderer && !activeRenderer->onKeyPress(key)) {
                return false;
            }
        }
        return true;
    }

    bool Scene::onKeyRelease(InputDeviceKey key) {
        for (auto* activeRenderer : std::initializer_list<Renderer*>{activeUiRenderer, activeRenderer3d}) {
            if (activeRenderer && !activeRenderer->onKeyRelease(key)) {
                return false;
            }
        }
        return true;
    }

    bool Scene::onChar(char32_t unicodeCharacter) {
        for (auto* activeRenderer : std::initializer_list<Renderer*>{activeUiRenderer, activeRenderer3d}) {
            if (activeRenderer && !activeRenderer->onChar(unicodeCharacter)) {
                return false;
            }
        }
        return true;
    }

    bool Scene::onMouseMove(double mouseX, double mouseY, double deltaMouseX, double deltaMouseY) {
        for (auto* activeRenderer : std::initializer_list<Renderer*>{activeUiRenderer, activeRenderer3d}) {
            if (activeRenderer && !activeRenderer->onMouseMove(mouseX, mouseY, deltaMouseX, deltaMouseY)) {
                return false;
            }
        }
        return true;
    }

    bool Scene::onScroll(double offsetY) {
        for (auto* activeRenderer : std::initializer_list<Renderer*>{activeUiRenderer, activeRenderer3d}) {
            if (activeRenderer && !activeRenderer->onScroll(offsetY)) {
                return false;
            }
        }
        return true;
    }

    void Scene::display() {
        {
            ScopeProfiler sp(Profiler::graphic(), "sceneDisplay");

            graphicsApiService->frameStart(frameCount);
            handleFpsLimiter();
            computeFps();
            float dt = getDeltaTime();

            unsigned int screenRenderingOrder = 0;
            screenRenderTarget.disableAllProcessors();
            for (auto* activeRenderer: std::initializer_list<Renderer *>{activeRenderer3d, activeUiRenderer}) {
                if (activeRenderer) {
                    screenRenderingOrder++;
                    activeRenderer->prepareRendering(frameCount, dt, screenRenderingOrder);
                }
            }
            screenRenderTarget.render(frameCount, 0);

            ResourceContainer::instance().cleanResources();
            graphicsApiService->frameEnd();
            frameCount++;
        }

        if (DEBUG_PROFILE_FRAME_BY_FRAME) {
            Profiler::graphic().log();
        }
    }

}
