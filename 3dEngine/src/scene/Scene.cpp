#include <string>

#include <scene/Scene.h>
#include <graphic/render/target/RenderTarget.h>
#include <graphic/render/target/screenshot/ScreenshotService.h>

namespace urchin {

    //static
    float Scene::STARTUP_FPS = 1000.0f; //high number of FPS to avoid pass through the ground at startup
    float Scene::FPS_REFRESH_TIME_IN_MS = 4.0f;
    float Scene::DISPLAY_FPS_REFRESH_TIME_IN_MS = 400.0f;

    Scene::Scene(const std::vector<std::string>& windowRequiredExtensions, std::unique_ptr<SurfaceCreator> surfaceCreator, std::unique_ptr<FramebufferSizeRetriever> framebufferSizeRetriever) :
            framebufferSizeRetriever(std::move(framebufferSizeRetriever)),
            i18nService(std::make_unique<I18nService>()),
            sceneWidth(0),
            sceneHeight(0),
            previousTime(),
            fps(STARTUP_FPS),
            fpsForDisplay((unsigned int)STARTUP_FPS),
            screenRenderTarget(std::make_unique<ScreenRender>("screen", RenderTarget::NO_DEPTH_ATTACHMENT)),
            activeRenderer3d(nullptr),
            activeUiRenderers(nullptr) {
        //scene properties
        this->framebufferSizeRetriever->getFramebufferSizeInPixel(sceneWidth, sceneHeight);

        //initialize
        SignalHandler::instance().initialize();
        GraphicService::instance().initialize(windowRequiredExtensions, std::move(surfaceCreator), *this->framebufferSizeRetriever);
        screenRenderTarget->initialize();
    }

    Scene::~Scene() {
        screenRenderTarget->cleanup();
        Profiler::graphic().log();
    }

    void Scene::updateVerticalSync(bool verticalSyncEnabled) {
        screenRenderTarget->updateVerticalSync(verticalSyncEnabled);
    }

    void Scene::onResize() {
        unsigned int newSceneWidth, newSceneHeight;
        framebufferSizeRetriever->getFramebufferSizeInPixel(newSceneWidth, newSceneHeight);

        if ((newSceneWidth > 1 && newSceneHeight > 1) && (sceneWidth != newSceneWidth || sceneHeight != newSceneHeight)) {
            Logger::instance().logInfo("Resize scene to " + std::to_string(newSceneWidth) + "x" + std::to_string(newSceneHeight));

            //scene properties
            this->sceneWidth = newSceneWidth;
            this->sceneHeight = newSceneHeight;

            //renderer
            screenRenderTarget->onResize();
            for (auto& renderer3d : renderers3d) {
                renderer3d->onResize(sceneWidth, sceneHeight);
            }
            for (auto& uiRenderer : uiRenderers) {
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

    I18nService& Scene::getI18nService() const {
        return *i18nService;
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
        return 1.0f / getFps();
    }

    void Scene::computeFps() {
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

    Renderer3d& Scene::newRenderer3d(bool enable) {
        auto renderer3d = std::make_unique<Renderer3d>(*screenRenderTarget);
        if (enable) {
            enableRenderer3d(renderer3d.get());
        }

        renderers3d.push_back(std::move(renderer3d));
        return *renderers3d.back();
    }

    void Scene::enableRenderer3d(Renderer3d* renderer3d) {
        if (activeRenderer3d && activeRenderer3d != renderer3d) {
            activeRenderer3d->onDisable();
        }
        activeRenderer3d = renderer3d;
    }

    void Scene::removeRenderer3d(Renderer3d* renderer3d) {
        if (activeRenderer3d == renderer3d) {
            activeRenderer3d = nullptr;
        }
        renderers3d.erase(std::remove_if(renderers3d.begin(), renderers3d.end(), [&](auto& p){return renderer3d == p.get();}), renderers3d.end());
    }

    Renderer3d* Scene::getActiveRenderer3d() const {
        return activeRenderer3d;
    }

    UIRenderer& Scene::newUIRenderer(bool enable) {
        auto uiRenderer = std::make_unique<UIRenderer>(*screenRenderTarget, *i18nService);
        if (enable) {
            enableUIRenderer(uiRenderer.get());
        }

        uiRenderers.push_back(std::move(uiRenderer));
        return *uiRenderers.back();
    }

    void Scene::enableUIRenderer(UIRenderer* uiRenderer) {
        if (activeUiRenderers && activeUiRenderers != uiRenderer) {
            activeUiRenderers->onDisable();
        }
        activeUiRenderers = uiRenderer;
    }

    void Scene::removeUIRenderer(UIRenderer* uiRenderer) {
        if (activeUiRenderers == uiRenderer) {
            activeUiRenderers = nullptr;
        }
        uiRenderers.erase(std::remove_if(uiRenderers.begin(), uiRenderers.end(), [&](auto& p){return uiRenderer == p.get();}), uiRenderers.end());
    }

    UIRenderer* Scene::getActiveUIRenderer() const {
        return activeUiRenderers;
    }

    void Scene::takeScreenShot(unsigned int width, unsigned int height) const {
        ScreenshotService::instance().takeScreenshot(*screenRenderTarget, width, height);
    }

    bool Scene::onKeyPress(unsigned int key) {
        for (auto* activeRenderer : std::initializer_list<Renderer*>{activeUiRenderers, activeRenderer3d}) {
            if (activeRenderer && !activeRenderer->onKeyPress(key)) {
                return false;
            }
        }
        return true;
    }

    bool Scene::onKeyRelease(unsigned int key) {
        for (auto* activeRenderer : std::initializer_list<Renderer*>{activeUiRenderers, activeRenderer3d}) {
            if (activeRenderer && !activeRenderer->onKeyRelease(key)) {
                return false;
            }
        }
        return true;
    }

    bool Scene::onChar(char32_t unicodeCharacter) {
        for (auto* activeRenderer : std::initializer_list<Renderer*>{activeUiRenderers, activeRenderer3d}) {
            if (activeRenderer && !activeRenderer->onChar(unicodeCharacter)) {
                return false;
            }
        }
        return true;
    }

    bool Scene::onMouseMove(double mouseX, double mouseY) {
        for (auto* activeRenderer : std::initializer_list<Renderer*>{activeUiRenderers, activeRenderer3d}) {
            if (activeRenderer && !activeRenderer->onMouseMove(mouseX, mouseY)) {
                return false;
            }
        }
        return true;
    }

    void Scene::display() {
        ScopeProfiler sp(Profiler::graphic(), "sceneMgrDisplay");

        //fps
        computeFps();
        float dt = getDeltaTime();

        //renderer
        screenRenderTarget->disableAllRenderers();
        for (auto* activeRenderer : std::initializer_list<Renderer*>{activeRenderer3d, activeUiRenderers}) {
            if (activeRenderer) {
                activeRenderer->prepareRendering(dt);
            }
        }
        screenRenderTarget->render();
    }

}
