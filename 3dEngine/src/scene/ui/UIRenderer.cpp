#include <algorithm>
#include <utility>
#include <queue>

#include <scene/ui/UIRenderer.h>
#include <scene/ui/displayer/WidgetSetDisplayer.h>
#include <resources/ResourceRetriever.h>
#include <resources/font/Font.h>
#include <graphics/render/shader/ShaderBuilder.h>

namespace urchin {

    //debug parameters
    bool DEBUG_DISPLAY_FONT_TEXTURE = false;

    UIRenderer::UIRenderer(float gammaFactor, RenderTarget& renderTarget, I18nService& i18nService) :
            Renderer(gammaFactor),
            renderTarget(renderTarget),
            i18nService(i18nService),
            clipboard(std::make_unique<ClipboardLocal>()),
            uiResolution((int)renderTarget.getWidth(), (int)renderTarget.getHeight()),
            rawMouseX(0.0),
            rawMouseY(0.0),
            bCanInteractWithUi(true),
            widgetSetDisplayer(std::make_unique<WidgetSetDisplayer>(*this)) {
        if (renderTarget.isValidRenderTarget()) {
            uiShader = ShaderBuilder::createShader("ui.vert.spv", "", "ui.frag.spv");
        } else {
            uiShader = ShaderBuilder::createNullShader();
        }
    }

    UIRenderer::~UIRenderer() {
        if (ui3dData && ui3dData->camera) {
            ui3dData->camera->removeObserver(this, Camera::POSITION_UPDATED);
        }
        removeAllWidgets();
    }

    void UIRenderer::setupUi3d(Camera* camera, const Transform<float>& transform, const Point2<int>& uiResolution, const Point2<float>& uiSize, float ambient) {
        if (!widgets.empty()) {
            throw std::runtime_error("UI renderer cannot be initialized for UI 3d because widgets already exist");
        }
        if (transform.hasScaling()) {
            throw std::runtime_error("Transform for UI 3d must have a scale of 1 (scale: " + StringUtil::toString(transform.getScale()) + ")");
        }
        if (!MathFunction::isEqual((float)uiResolution.X / (float)uiResolution.Y, uiSize.X / uiSize.Y, 0.01f)) {
            Logger::instance().logWarning("UI size (" + StringUtil::toString(uiSize) + ") and UI resolution (" + StringUtil::toString(uiResolution) + ") have not the same proportion");
        }

        ui3dData = std::make_unique<UI3dData>();

        float xScale = uiSize.X / (float)uiResolution.X;
        float yScale = -uiSize.Y / (float)uiResolution.Y; //negate for flip on Y axis
        Matrix4 uiViewMatrix(xScale, 0.0f, 0.0f, 0.0f,
                             0.0f, yScale, 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 1.0f);
        ui3dData->modelMatrix = transform.getTransformMatrix() * uiViewMatrix;

        ui3dData->normalMatrix = ui3dData->modelMatrix.inverse().transpose();

        Point3<float> topLeft = (ui3dData->modelMatrix * Point4<float>(0.0f, 0.0f, 0.0f, 1.0f)).toPoint3();
        Point3<float> topRight = (ui3dData->modelMatrix * Point4<float>((float)uiResolution.X, 0.0f, 0.0f, 1.0f)).toPoint3();
        Point3<float> bottomRight = (ui3dData->modelMatrix * Point4<float>((float)uiResolution.X, (float)uiResolution.Y, 0.0f, 1.0f)).toPoint3();
        ui3dData->uiPlane.buildFrom3Points(topLeft, bottomRight, topRight);
        ui3dData->uiPosition = (topLeft + bottomRight) / 2.0f;
        ui3dData->uiSphereBounding = Sphere<float>(ui3dData->uiPosition.distance(bottomRight), ui3dData->uiPosition);

        if (renderTarget.isValidRenderTarget()) {
            std::vector<std::size_t> variablesSize = {sizeof(ambient)};
            auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &ambient);
            this->uiShader = ShaderBuilder::createShader("ui3d.vert.spv", "", "ui3d.frag.spv", std::move(shaderConstants));
        }

        onResize((unsigned int)uiResolution.X, (unsigned int)uiResolution.Y);
        if (camera) {
            onCameraProjectionUpdate(*camera);
        }
    }

    void UIRenderer::setupClipboard(std::unique_ptr<Clipboard> clipboard) {
        if (!clipboard) {
            throw std::runtime_error("The provided clipboard can not be null");
        }
        this->clipboard = std::move(clipboard);
    }

    void UIRenderer::onCameraProjectionUpdate(Camera& camera) {
        if (!ui3dData) {
            throw std::runtime_error("UI renderer has not been initialized for UI 3d");
        }
        ui3dData->camera = &camera;
        ui3dData->camera->addObserver(this, Camera::POSITION_UPDATED);

        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            widgets[(std::size_t)i]->onCameraProjectionUpdate();
        }
    }

    void UIRenderer::setMaximumInteractiveDistance(float maxInteractiveDistance) const {
        if (!ui3dData) {
            throw std::runtime_error("UI renderer has not been initialized for UI 3d");
        }
        ui3dData->maxInteractiveDistance = maxInteractiveDistance;
    }

    void UIRenderer::setPointerType(UI3dPointerType pointerType) const {
        if (!ui3dData) {
            throw std::runtime_error("UI renderer has not been initialized for UI 3d");
        }
        ui3dData->pointerType = pointerType;
    }

    void UIRenderer::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        uiResolution = Point2<int>((int)sceneWidth, (int)sceneHeight);

        //widgets resize
        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            widgets[(std::size_t)i]->onResize();
        }

        //debug
        if (DEBUG_DISPLAY_FONT_TEXTURE) {
            auto font = ResourceRetriever::instance().getResource<Font>("UI/fontText.ttf", {{"fontSize", "16"}, {"fontColor", "1.0 1.0 1.0"}});

            auto textureDisplayer = std::make_unique<TextureRenderer>(font->getTexture(), TextureRenderer::DEFAULT_VALUE);
            textureDisplayer->setPosition(TextureRenderer::USER_DEFINED_X, TextureRenderer::USER_DEFINED_Y);
            textureDisplayer->setSize(20.0f, (float)font->getDimensionTexture() + 20.0f, 20.0f, (float)font->getDimensionTexture() + 20.0f);
            textureDisplayer->enableTransparency();
            textureDisplayer->initialize("[DEBUG] font texture", renderTarget, sceneWidth, sceneHeight, -1.0f, -1.0f);
            debugFont = std::move(textureDisplayer);
        }
    }

    void UIRenderer::notify(Observable* observable, int notificationType) {
        if (const auto* widget = dynamic_cast<Widget*>(observable)) {
            if (notificationType == Widget::SET_IN_FOREGROUND) {
                auto itFind = std::ranges::find_if(widgets, [&widget](const auto& o){return widget == o.get();});
                std::shared_ptr<Widget> widgetPtr = *itFind;
                widgets.erase(itFind);
                widgets.push_back(widgetPtr);

                //reset the others widgets
                for (long i = (long)widgets.size() - 2; i >= 0; --i) {
                    widgets[(std::size_t)i]->onResetState();
                }
            }
        } else if (dynamic_cast<Camera*>(observable)) {
            if (notificationType == Camera::POSITION_UPDATED) {
                if (ui3dData) {
                    onCursorMove();
                }
            }
        }
    }

    bool UIRenderer::onKeyPress(InputDeviceKey key) {
        if (bCanInteractWithUi) {
            //keep a temporary copy of the widgets in case the underlying action goal is to destroy the widgets
            std::vector<std::shared_ptr<Widget>> widgetsCopy = widgets;
            for (long i = (long) widgetsCopy.size() - 1; i >= 0; --i) {
                if (!widgetsCopy[(std::size_t) i]->onKeyPress(key)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool UIRenderer::onKeyRelease(InputDeviceKey key) {
        if (bCanInteractWithUi) {
            //keep a temporary copy of the widgets in case the underlying action goal is to destroy the widgets
            std::vector<std::shared_ptr<Widget>> widgetsCopy = widgets;
            for (long i = (long) widgetsCopy.size() - 1; i >= 0; --i) {
                if (!widgetsCopy[(std::size_t) i]->onKeyRelease(key)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool UIRenderer::onChar(char32_t unicodeCharacter) {
        if (bCanInteractWithUi && UnicodeUtil::isCharacterDisplayable(unicodeCharacter)) {
            for (long i = (long) widgets.size() - 1; i >= 0; --i) {
                if (!widgets[(std::size_t) i]->onChar(unicodeCharacter)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool UIRenderer::onMouseMove(double mouseX, double mouseY) {
        this->rawMouseX = mouseX;
        this->rawMouseY = mouseY;

        return onCursorMove();
    }

    bool UIRenderer::onCursorMove() {
        Point2<int> adjustedMouseCoordinate;
        bCanInteractWithUi = adjustMouseCoordinates(adjustedMouseCoordinate);

        if (bCanInteractWithUi) {
            for (long i = (long)widgets.size() - 1; i >= 0; --i) {
                if (!widgets[(std::size_t)i]->onMouseMove(adjustedMouseCoordinate.X, adjustedMouseCoordinate.Y)) {
                    return false;
                }
            }
        } else {
            onDisable();
        }
        return true;
    }

    /**
     * @param adjustedMouseCoord [out] Adjusted mouse coordinates
     * @return True when camera is near to UI to interact with it
     */
    bool UIRenderer::adjustMouseCoordinates(Point2<int>& adjustedMouseCoord) const {
        if (ui3dData) {
            if (ui3dData->maxInteractiveDistance <= 0.0f) {
                return false; //interaction disabled
            }
            if (!ui3dData->camera) {
                return false; //camera not setup yet
            }
            if (ui3dData->uiPosition.squareDistance(ui3dData->camera->getPosition()) > ui3dData->maxInteractiveDistance * ui3dData->maxInteractiveDistance) {
                return false; //camera too far from the UI
            }
            if (ui3dData->camera->getView().dotProduct(ui3dData->uiPlane.getNormal()) > 0.0f) {
                return false; //camera does not face to the UI
            }
            if (ui3dData->uiPosition.vector(ui3dData->camera->getPosition()).dotProduct(ui3dData->uiPlane.getNormal()) < 0.0f) {
                return false; //camera is behind the UI
            }

            Point2<float> pointer;
            if (ui3dData->pointerType == UI3dPointerType::MOUSE) {
                pointer = Point2<float>((float)rawMouseX, (float)rawMouseY);
            } else if (ui3dData->pointerType == UI3dPointerType::SCREEN_CENTER) {
                pointer = Point2<float>((float)renderTarget.getWidth() / 2.0f, (float)renderTarget.getHeight() / 2.0f);
            } else {
                 throw std::runtime_error("Unknown pointer type: " + std::to_string(ui3dData->pointerType));
            }

            Line3D<float> viewLine = CameraSpaceService(*ui3dData->camera).screenPointToLine(Point2<float>(pointer.X, pointer.Y));
            bool hasIntersection = false;
            Point3<float> uiHitPoint = ui3dData->uiPlane.intersectPoint(viewLine, hasIntersection);
            if (!hasIntersection) {
                return false; //camera is parallel to the UI plane
            }

            Point3<float> intersectionPointClipSpace = (ui3dData->camera->getProjectionViewMatrix() * Point4<float>(uiHitPoint)).toPoint3();
            float clipSpaceX = (2.0f * pointer.X) / ((float) renderTarget.getWidth()) - 1.0f;
            float clipSpaceY = (2.0f * pointer.Y) / ((float) renderTarget.getHeight()) - 1.0f;
            Point4 mousePos(clipSpaceX, clipSpaceY, intersectionPointClipSpace.Z, 1.0f);

            Matrix4<float> uiInverseMatrix = (ui3dData->camera->getProjectionViewMatrix() * ui3dData->modelMatrix).inverse();
            Point3<float> mouseScreen = (uiInverseMatrix * mousePos).toPoint3();
            if (mouseScreen.X < 0.0f || mouseScreen.X > (float)uiResolution.X || mouseScreen.Y < 0.0f || mouseScreen.Y > (float)uiResolution.Y) {
                return false; //cursor outside UI plane
            }

            adjustedMouseCoord = Point2<int>((int)mouseScreen.X, (int)mouseScreen.Y);
            return true;
        }

        adjustedMouseCoord = Point2<int>((int)rawMouseX, (int)rawMouseY);
        return true;
    }

    bool UIRenderer::onScroll(double offsetY) {
        if (bCanInteractWithUi) {
            for (long i = (long) widgets.size() - 1; i >= 0; --i) {
                if (!widgets[(std::size_t) i]->onScroll(offsetY)) {
                    return false;
                }
            }
        }
        return true;
    }

    void UIRenderer::onDisable() {
        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            widgets[(std::size_t)i]->onResetState();
        }
    }

    RenderTarget& UIRenderer::getRenderTarget() const {
        return renderTarget;
    }

    I18nService& UIRenderer::getI18nService() const {
        return i18nService;
    }

    Clipboard& UIRenderer::getClipboard() {
        return *clipboard;
    }

    const Point2<int>& UIRenderer::getUiResolution() const {
        return uiResolution;
    }

    Shader& UIRenderer::getShader() const {
        return *uiShader;
    }

    UI3dData* UIRenderer::getUi3dData() const {
        return ui3dData.get();
    }

    bool UIRenderer::canInteractWithUi() const {
        return bCanInteractWithUi;
    }

    WidgetSetDisplayer& UIRenderer::getWidgetSetDisplayer() const {
        return *widgetSetDisplayer;
    }

    void UIRenderer::applyUpdatedGammaFactor() {
        for (const auto& widget : widgets) {
            widget->applyUpdatedGammaFactor();
        }
    }

    void UIRenderer::addWidget(const std::shared_ptr<Widget>& widget) {
        if (widget->getParent()) {
            throw std::runtime_error("Cannot add a widget having a parent to the UI renderer");
        }
        widgets.push_back(widget);

        widget->initialize(*this);
        widget->addObserver(this, Widget::SET_IN_FOREGROUND);
    }

    void UIRenderer::removeWidget(const Widget& widget) {
        auto itFind = std::ranges::find_if(widgets, [&widget](const auto& o){ return &widget == o.get(); });
        if (itFind == widgets.end()) {
            throw std::runtime_error("The provided widget is not widget of this UI renderer");
        }
        (*itFind)->uninitialize();
        widgets.erase(itFind);
    }

    void UIRenderer::removeAllWidgets() {
        for (const auto& widget : widgets) {
            widget->uninitialize();
        }
        widgets.clear();
    }

    bool UIRenderer::isWidgetExist(const Widget& widget) const {
        return std::ranges::find_if(widgets, [&widget](const auto& o){ return &widget == o.get(); }) != widgets.end();
    }

    const std::vector<std::shared_ptr<Widget>>& UIRenderer::getWidgets() const {
        return widgets;
    }

    void UIRenderer::prepareRendering(std::uint32_t, float dt, unsigned int& screenRenderingOrder) {
        prepareRendering(dt, screenRenderingOrder, Matrix4<float>());
    }

    void UIRenderer::prepareRendering(float dt, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) const {
        ScopeProfiler sp(Profiler::graphic(), "uiPreRendering");

        bool isUiVisible = true;
        if (ui3dData && (
                ui3dData->uiPosition.vector(ui3dData->camera->getPosition()).dotProduct(ui3dData->uiPlane.getNormal()) < 0.0f || //camera is behind the UI
                !ui3dData->camera->getFrustum().collideWithSphere(ui3dData->uiSphereBounding)) //UI not in frustum
        ) {
            isUiVisible = false;
        }

        if (isUiVisible) {
            widgetsToRender.clear();
            prepareWidgets(dt, widgets);
            widgetSetDisplayer->updateWidgets(widgetsToRender);
            widgetSetDisplayer->prepareRendering(renderingOrder, projectionViewMatrix);
        }

        //debug
        if (debugFont) {
            renderingOrder++;
            debugFont->prepareRendering(renderingOrder);
        }
    }

    void UIRenderer::prepareWidgets(float dt, const std::vector<std::shared_ptr<Widget>>& rootWidgets) const { //TODO add test
        std::queue<Widget*> widgetsQueue; //TODO handle memory dyn allocation

        //for (const auto& widget : rootWidgets) { //TODO rollback
        for(auto it = rootWidgets.rbegin(); it != rootWidgets.rend(); ++it) {
            if ((*it)->isVisible()) {
                widgetsQueue.push((*it).get());
            }
        }

        while (!widgetsQueue.empty()) {
            Widget* widget = widgetsQueue.front();
            widgetsQueue.pop();

            widget->prepareWidgetRendering(dt);
            if (widget->requireRenderer()) {
                widgetsToRender.push_back(widget);
            }

            if (widget->getWidgetType() == WidgetType::WINDOW) [[unlikely]] {
                prepareWidgets(dt, widget->getChildren());
            } else {
                //for (const auto& widgetChild: widget->getChildren()) { //TODO rollback
                for(auto it = widget->getChildren().rbegin(); it != widget->getChildren().rend(); ++it) {
                    if ((*it)->isVisible()) {
                        widgetsQueue.push((*it).get());
                    }
                }
            }
        }
    }

}
