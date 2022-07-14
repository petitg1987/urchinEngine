#include <algorithm>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/container/Container.h>
#include <scene/ui/UIRenderer.h>
#include <scene/InputDeviceKey.h>

namespace urchin {

    Widget::Widget(Position position, Size size) :
            uiRenderer(nullptr),
            parent(nullptr),
            widgetState(Widget::DEFAULT),
            position(position),
            size(size),
            scale(Vector2<float>(1.0f, 1.0f)),
            rotationZ(0.0f),
            alphaFactor(1.0f),
            scissorEnabled(false),
            bIsVisible(true),
            mouseX(0),
            mouseY(0) {

    }

    Widget::~Widget() {
        Widget::detachChildren();
    }

    void Widget::initialize(UIRenderer& uiRenderer) {
        ScopeProfiler sp(Profiler::graphic(), "widgetInit");

        this->uiRenderer = &uiRenderer;

        createOrUpdateWidget();
        for (const auto& child : children) {
            child->initialize(uiRenderer);
        }
        refreshScissor(false /* children call already treated via child->initialize */);
    }

    bool Widget::isInitialized() const {
        return uiRenderer != nullptr;
    }

    void Widget::onResize() {
        createOrUpdateWidget();
        for (const auto& child : children) {
            child->onResize();
        }
        refreshScissor(false /* children call already treated via child->onResize */);
    }

    void Widget::onCameraProjectionUpdate() {
        createOrUpdateWidget();
        for (const auto& child : children) {
            child->onCameraProjectionUpdate();
        }
        refreshScissor(false /* children call already treated via child->onCameraProjectionUpdate */);
    }

    std::shared_ptr<GenericRendererBuilder> Widget::setupUiRenderer(std::string name, ShapeType shapeType, bool hasTransparency) {
        assert(isInitialized());
        auto rendererBuilder = GenericRendererBuilder::create(std::move(name), uiRenderer->getRenderTarget(), uiRenderer->getShader(), shapeType);

        Matrix4<float> normalMatrix;
        Matrix4<float> projectionViewModelMatrix;
        if (uiRenderer->getUi3dData()) {
            rendererBuilder->enableDepthTest();
            rendererBuilder->enableDepthWrite();

            //Always active transparency to ensure that emissive factor stay unchanged (see fragment shader for more details).
            //Transparency is only working when a transparent widget is displayed above another widget.
            //Transparency to the scene is currently not supported because the UI is written on an RGB channel (no alpha).
            rendererBuilder->enableTransparency({
                BlendFunction::build(BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::ZERO, BlendFactor::ONE),
                BlendFunction::buildBlendDisabled(),
                BlendFunction::buildBlendDisabled()
            });
            normalMatrix = uiRenderer->getUi3dData()->normalMatrix;
        } else {
            if (hasTransparency) {
                rendererBuilder->enableTransparency({BlendFunction::buildDefault()});
            }
        }

        rendererBuilder->addUniformData(sizeof(normalMatrix), &normalMatrix); //binding 0
        rendererBuilder->addUniformData(sizeof(projectionViewModelMatrix), &projectionViewModelMatrix); //binding 1
        rendererBuilder->addUniformData(sizeof(alphaFactor), &alphaFactor); //binding 2

        refreshCoordinates();
        rendererBuilder->addData(vertexCoord);
        rendererBuilder->addData(textureCoord);

        return rendererBuilder;
    }

    TextureParam::Anisotropy Widget::getTextureAnisotropy() const {
        if (uiRenderer->getUi3dData()) {
            return TextureParam::Anisotropy::ANISOTROPY;
        }
        return TextureParam::Anisotropy::NO_ANISOTROPY;
    }

    void Widget::updateProperties(GenericRenderer* renderer, const Matrix4<float>& projectionViewMatrix, const Vector2<float>& translateVector) const {
        Matrix4<float> projectionViewModelMatrix;

        float zBias = 0.0f;
        if (uiRenderer->getUi3dData()) {
            float squareDistanceUiToCamera = uiRenderer->getUi3dData()->uiPosition.squareDistance(uiRenderer->getUi3dData()->camera->getPosition());
            zBias = (float)computeDepthLevel() * 0.0003f * std::clamp(squareDistanceUiToCamera, 0.5f, 6.0f);
            projectionViewModelMatrix = projectionViewMatrix * uiRenderer->getUi3dData()->modelMatrix;
        } else {
            Matrix4 orthogonalMatrix( //orthogonal matrix with origin at top left screen
                    2.0f / (float) uiRenderer->getUiResolution().X, 0.0f, -1.0f, 0.0f,
                    0.0f, 2.0f / (float) uiRenderer->getUiResolution().Y, -1.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
            projectionViewModelMatrix = orthogonalMatrix;
        }

        //Equivalent to 4 multiplied matrices: D * C * B * A
        // A) Translation of the widget center to the origin (transOriginX, transOriginY)
        // B) Scale widget (scale.X, scale.Y)
        // C) Rotate widget (sinRotate, cosRotate)
        // D) Translation rollback of "a" + translation for positioning (transX, transY, zBias)
        float transX = translateVector.X + getWidth() / 2.0f;
        float transY = translateVector.Y + getHeight() / 2.0f;
        float transOriginX = -getWidth() / 2.0f;
        float transOriginY = -getHeight() / 2.0f;
        float sinRotate = std::sin(rotationZ);
        float cosRotate = std::cos(rotationZ);
        Matrix4<float> translateScaleMatrix(
                scale.X * cosRotate, scale.Y * -sinRotate, 0.0f, transX + (transOriginX * scale.X * cosRotate) + (transOriginY * scale.Y * -sinRotate),
                scale.X * sinRotate, scale.Y * cosRotate, 0.0f, transY + (transOriginX * scale.X * sinRotate) + (transOriginY * scale.Y * cosRotate),
                0.0f, 0.0, 1.0f, zBias,
                0.0f, 0.0f, 0.0f, 1.0f);

        projectionViewModelMatrix *= translateScaleMatrix;
        renderer->updateUniformData(1, &projectionViewModelMatrix);
        renderer->updateUniformData(2, &alphaFactor);
        if (scissorEnabled) {
            renderer->updateScissor(scissorOffset, scissorSize);
        }
    }

    I18nService& Widget::getI18nService() const {
        if (!uiRenderer) {
            throw std::runtime_error("Internationalization service not available because the widget is not initialized");
        }
        return uiRenderer->getI18nService();
    }

    UI3dData* Widget::getUi3dData() const {
        if (!uiRenderer) {
            throw std::runtime_error("UI 3d data not available because the widget is not initialized");
        }
        return uiRenderer->getUi3dData();
    }

    Clipboard& Widget::getClipboard() const {
        if (!uiRenderer) {
            throw std::runtime_error("Clipboard not available because the widget is not initialized");
        }
        return uiRenderer->getClipboard();
    }

    Widget* Widget::getParent() const {
        return parent;
    }

    Container* Widget::getParentContainer() const {
        Widget *parent = getParent();
        while (parent != nullptr) {
            if (parent->getWidgetType() == WidgetType::CONTAINER) {
                return static_cast<Container*>(parent);
            }
            parent = parent->getParent();
        }
        return nullptr;
    }

    void Widget::addChild(const std::shared_ptr<Widget>& childWidget) {
        if (childWidget->getParent()) {
            throw std::runtime_error("Cannot add a widget which already have a parent");
        }

        childWidget->parent = this;
        children.push_back(childWidget);

        if (uiRenderer) {
            childWidget->initialize(*uiRenderer);
        }
    }

    const std::vector<std::shared_ptr<Widget>>& Widget::getChildren() const {
        return children;
    }

    void Widget::detachChild(Widget* childWidget) {
        if (childWidget) {
            auto itFind = std::ranges::find_if(children, [&childWidget](const auto& o) { return childWidget == o.get(); });
            if (itFind == children.end()) {
                throw std::runtime_error("The provided child widget is not a child of this widget");
            }

            childWidget->parent = nullptr;
            children.erase(itFind);
        }
    }

    void Widget::detachChildren() {
        for (auto it = children.begin(); it != children.end();) {
            (*it)->parent = nullptr;
            it = children.erase(it);
        }
    }

    void Widget::clearEventListeners() {
        eventListeners.clear();
    }

    void Widget::addEventListener(std::shared_ptr<EventListener> eventListener) {
        this->eventListeners.push_back(std::move(eventListener));
    }

    const std::vector<std::shared_ptr<EventListener>>& Widget::getEventListeners() const {
        return eventListeners;
    }

    Widget::WidgetState Widget::getWidgetState() const {
        return widgetState;
    }

    Point2<int> Widget::getSceneSize() const {
        if (!uiRenderer) {
            throw std::runtime_error("Scene size not available because the widget is not initialized");
        }
        return uiRenderer->getUiResolution();
    }

    void Widget::updatePosition(Position position) {
        if (this->getWidgetType() == WidgetType::CONTAINER && static_cast<Container*>(this)->isScrollable()) {
            throw std::runtime_error("Can not move a container: scissor update is not implemented");
        }
        this->position = position;

        for (const auto& child : children) {
            child->refreshScissor(true);
        }
    }

    Position Widget::getPosition() const {
        return position;
    }

    /**
    * @return Relative position X of the widget
    */
    float Widget::getPositionX() const {
        return widthLengthToPixel(position.getX(), position.getXType(), [&](){ return getPositionY(); });
    }

    /**
    * @return Relative position Y of the widget
    */
    float Widget::getPositionY() const {
        return heightLengthToPixel(position.getY(), position.getYType(), [&](){ return getPositionX(); });
    }

    const WidgetOutline& Widget::getOutline() const {
        return widgetOutline;
    }

    float Widget::getGlobalPositionX() const {
        float startPosition = 0;
        if (position.getRelativeTo() == RelativeTo::PARENT_LEFT_TOP
                || position.getRelativeTo() == RelativeTo::PARENT_LEFT_BOTTOM
                || position.getRelativeTo() == RelativeTo::PARENT_LEFT_CENTERY) { //left
            if (parent) {
                startPosition = parent->getGlobalPositionX() + (float)parent->getOutline().leftWidth;
            } else {
                startPosition = 0.0f;
            }
        } else if (position.getRelativeTo() == RelativeTo::PARENT_RIGHT_TOP
                || position.getRelativeTo() == RelativeTo::PARENT_RIGHT_BOTTOM
                || position.getRelativeTo() == RelativeTo::PARENT_RIGHT_CENTERY) { //right
            if (parent) {
                startPosition = parent->getGlobalPositionX() - (float)parent->getOutline().rightWidth + parent->getWidth();
            } else {
                startPosition = (float)getSceneSize().X;
            }
        } else if (position.getRelativeTo() == RelativeTo::PARENT_CENTER_XY
                || position.getRelativeTo() == RelativeTo::PARENT_CENTERX_TOP
                || position.getRelativeTo() == RelativeTo::PARENT_CENTERX_BOTTOM) { //center X
            if (parent) {
                startPosition = parent->getGlobalPositionX() + (float)parent->getOutline().leftWidth + parent->getWidth() / 2.0f;
            } else {
                startPosition = (float)getSceneSize().X / 2.0f;
            }
        }

        if (position.getReferencePoint() == RefPoint::RIGHT_TOP || position.getReferencePoint() == RefPoint::RIGHT_BOTTOM || position.getReferencePoint() == RefPoint::RIGHT_CENTERY) { //right
            startPosition -= getWidth();
        } else if (position.getReferencePoint() == RefPoint::CENTER_XY || position.getReferencePoint() == RefPoint::CENTERX_TOP || position.getReferencePoint() == RefPoint::CENTERX_BOTTOM) { //center X
            startPosition -= getWidth() / 2.0f;
        }

        return startPosition + getPositionX();
    }

    float Widget::getGlobalPositionY() const {
        float startPosition = 0;
        if (position.getRelativeTo() == RelativeTo::PARENT_LEFT_TOP
                || position.getRelativeTo() == RelativeTo::PARENT_RIGHT_TOP
                || position.getRelativeTo() == RelativeTo::PARENT_CENTERX_TOP) { //top
            if (parent) {
                startPosition = parent->getGlobalPositionY() + (float)parent->getOutline().topWidth;
            } else {
                startPosition = 0.0f;
            }
        } else if (position.getRelativeTo() == RelativeTo::PARENT_LEFT_BOTTOM
                || position.getRelativeTo() == RelativeTo::PARENT_RIGHT_BOTTOM
                || position.getRelativeTo() == RelativeTo::PARENT_CENTERX_BOTTOM) { //bottom
            if (parent) {
                startPosition = parent->getGlobalPositionY() - (float)parent->getOutline().bottomWidth + parent->getHeight();
            } else {
                startPosition = (float)getSceneSize().Y;
            }
        } else if (position.getRelativeTo() == RelativeTo::PARENT_CENTER_XY
                || position.getRelativeTo() == RelativeTo::PARENT_LEFT_CENTERY
                || position.getRelativeTo() == RelativeTo::PARENT_RIGHT_CENTERY) { //center Y
            if (parent) {
                startPosition = parent->getGlobalPositionY() + (float)parent->getOutline().topWidth + parent->getHeight() / 2.0f;
            } else {
                startPosition = (float)getSceneSize().Y / 2.0f;
            }
        }

        if (const auto* scrollable = dynamic_cast<Scrollable*>(parent)) {
            startPosition += (float)scrollable->getScrollShiftY();
        }

        if (position.getReferencePoint() == RefPoint::LEFT_BOTTOM || position.getReferencePoint() == RefPoint::RIGHT_BOTTOM || position.getReferencePoint() == RefPoint::CENTERX_BOTTOM) { //bottom
            startPosition -= getHeight();
        } else if (position.getReferencePoint() == RefPoint::CENTER_XY ||  position.getReferencePoint() == RefPoint::LEFT_CENTERY || position.getReferencePoint() == RefPoint::RIGHT_CENTERY) { //center Y
            startPosition -= getHeight() / 2.0f;
        }

        return startPosition + getPositionY();
    }

    void Widget::updateSize(Size size) {
        setSize(size); //TODO remove setSize ?
        refreshCoordinates();
        renderer->updateData(0, vertexCoord);
        refreshScissor(true);
    }

    void Widget::refreshCoordinates() {
        vertexCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(getWidth(), 0.0f), Point2<float>(getWidth(), getHeight()),
                Point2<float>(0.0f, 0.0f), Point2<float>(getWidth(), getHeight()), Point2<float>(0.0f, getHeight())
        };
        textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
    }

    void Widget::setSize(Size size) {
        this->size = size;
    }

    Size Widget::getSize() const {
        return size;
    }

    float Widget::getWidth() const {
        return widthLengthToPixel(size.getWidth(), size.getWidthType(), [&](){ return getHeight(); });
    }

    float Widget::getHeight() const {
        return heightLengthToPixel(size.getHeight(), size.getHeightType(), [&](){ return getWidth(); });
    }

    Rectangle2D<int> Widget::widgetRectangle() const {
        return Rectangle2D<int>(
                Point2<int>((int)getGlobalPositionX(), (int)getGlobalPositionY()),
                Point2<int>((int)getGlobalPositionX() + (int)getWidth(), (int)getGlobalPositionY() + (int)getHeight()));
    }

    void Widget::updateScale(const Vector2<float>& scale) {
        this->scale = scale;
    }

    const Vector2<float>& Widget::getScale() const {
        return scale;
    }

    void Widget::updateRotation(float rotationZ) {
        this->rotationZ = rotationZ;
    }

    float Widget::getRotation() const {
        return rotationZ;
    }

    void Widget::updateAlphaFactor(float alphaFactor) {
        this->alphaFactor = alphaFactor;
    }

    float Widget::getAlphaFactor() const {
        return alphaFactor;
    }

    float Widget::widthPixelToLength(float widthPixel, LengthType lengthType) const {
        if (lengthType == LengthType::SCREEN_PERCENT) {
            return (widthPixel / (float)getSceneSize().X) * 100.0f;
        } else if (lengthType == LengthType::CONTAINER_PERCENT) {
            const Container* parentContainer = getParentContainer();
            return (widthPixel / (getParentContainer()->getWidth() - (float)parentContainer->getOutline().leftWidth - (float)parentContainer->getOutline().rightWidth)) * 100.0f;
        } else if (lengthType == LengthType::PARENT_PERCENT) {
            return (widthPixel / (getParent()->getWidth() - (float)getParent()->getOutline().leftWidth - (float)getParent()->getOutline().rightWidth)) * 100.0f;
        } else if (lengthType == LengthType::PIXEL) {
            return widthPixel;
        }
        throw std::runtime_error("Unknown/unimplemented length type: " + std::to_string(lengthType));
    }

    float Widget::heightPixelToLength(float heightPixel, LengthType lengthType) const {
        if (lengthType == LengthType::SCREEN_PERCENT) {
            return (heightPixel / (float)getSceneSize().Y) * 100.0f;
        } else if (lengthType == LengthType::CONTAINER_PERCENT) {
            const Container* parentContainer = getParentContainer();
            return (heightPixel / (parentContainer->getHeight() - (float)parentContainer->getOutline().bottomWidth - (float)parentContainer->getOutline().topWidth)) * 100.0f;
        } else if (lengthType == LengthType::PARENT_PERCENT) {
            return (heightPixel / (getParent()->getHeight() - (float)getParent()->getOutline().bottomWidth - (float)getParent()->getOutline().topWidth)) * 100.0f;
        } else if (lengthType == LengthType::PIXEL) {
            return heightPixel;
        }
        throw std::runtime_error("Unknown/unimplemented length type: " + std::to_string(lengthType));
    }

    void Widget::setIsVisible(bool isVisible) {
        if (!isVisible) {
            onResetState();
        }
        this->bIsVisible = isVisible;
    }

    bool Widget::isVisible() const {
        return bIsVisible;
    }

    bool Widget::onKeyPress(InputDeviceKey key) {
        bool propagateEvent = true;
        if (isVisible()) {
            bool widgetStateUpdated = handleWidgetKeyPress(key);
            propagateEvent = onKeyPressEvent(key);

            if (widgetStateUpdated && widgetState == Widget::CLICKING) {
                for (const auto& eventListener : eventListeners) {
                    propagateEvent &= eventListener->onMouseLeftClick(this);
                }
            }

            //keep a temporary copy of the widgets in case the underlying action goal is to destroy the widgets
            std::vector<std::shared_ptr<Widget>> childrenCopy = children;
            for (const auto& child : childrenCopy) {
                if (!child->onKeyPress(key)) {
                    return false;
                }
            }
        }
        return propagateEvent;
    }

    bool Widget::handleWidgetKeyPress(InputDeviceKey key) {
        bool widgetStateUpdated = false;
        if (key == InputDeviceKey::MOUSE_LEFT) {
            //In some rare cases, the state could be different from FOCUS:
            // - Widget has just been made visible and mouse has not moved yet
            // - UIRenderer has just been enabled and mouse has not moved yet
            if (isMouseOnWidget(mouseX, mouseY) && widgetState == FOCUS) {
                widgetState = CLICKING;
                widgetStateUpdated = true;
            }
        }
        return widgetStateUpdated;
    }

    bool Widget::onKeyPressEvent(InputDeviceKey) {
        return true;
    }

    bool Widget::onKeyRelease(InputDeviceKey key) {
        bool propagateEvent = true;
        if (isVisible()) {
            bool widgetStateUpdated = handleWidgetKeyRelease(key);
            propagateEvent = onKeyReleaseEvent(key);

            if (widgetStateUpdated && widgetState == Widget::FOCUS) {
                for (const auto& eventListener : eventListeners) {
                    propagateEvent &= eventListener->onMouseLeftClickRelease(this);
                }
            } else if (widgetStateUpdated && widgetState == Widget::DEFAULT) {
                for (const auto& eventListener : eventListeners) {
                    propagateEvent &= eventListener->onFocusLost(this);
                }
            }

            //keep a temporary copy of the widgets in case the underlying action goal is to destroy the widgets
            std::vector<std::shared_ptr<Widget>> childrenCopy = children;
            for (const auto& child : childrenCopy) {
                if (!child->onKeyRelease(key)) {
                    return false;
                }
            }
        }
        return propagateEvent;
    }

    bool Widget::handleWidgetKeyRelease(InputDeviceKey key) {
        bool widgetStateUpdated = false;
        if (key == InputDeviceKey::MOUSE_LEFT) {
            if (isMouseOnWidget(mouseX, mouseY)) {
                if (widgetState == CLICKING) {
                    widgetState = FOCUS;
                    widgetStateUpdated = true;
                }
            } else {
                if (widgetState == CLICKING) {
                    widgetState = DEFAULT;
                    widgetStateUpdated = true;
                }
            }
        }
        return widgetStateUpdated;
    }

    bool Widget::onKeyReleaseEvent(InputDeviceKey) {
        return true;
    }

    bool Widget::onChar(char32_t unicodeCharacter) {
        bool propagateEvent = true;
        if (isVisible()) {
            propagateEvent = onCharEvent(unicodeCharacter);

            for (const auto& child : children) {
                if (!child->onChar(unicodeCharacter)) {
                    return false;
                }
            }
        }
        return propagateEvent;
    }

    bool Widget::onCharEvent(char32_t) {
        return true;
    }

    bool Widget::onMouseMove(int mouseX, int mouseY) {
        this->mouseX = mouseX;
        this->mouseY = mouseY;

        bool propagateEvent = true;
        if (isVisible()) {
            bool widgetStateUpdated = handleWidgetMouseMove(mouseX, mouseY);
            propagateEvent = onMouseMoveEvent(mouseX, mouseY);

            if (widgetStateUpdated && widgetState == Widget::FOCUS) {
                for (const auto& eventListener : eventListeners) {
                    propagateEvent &= eventListener->onFocus(this);
                }
            } else if (widgetStateUpdated && widgetState == Widget::DEFAULT) {
                for (const auto& eventListener : eventListeners) {
                    propagateEvent &= eventListener->onFocusLost(this);
                }
            }

            for (const auto& child : children) {
                if (!child->onMouseMove(mouseX, mouseY)) {
                    return false;
                }
            }
        }
        return propagateEvent;
    }

    bool Widget::handleWidgetMouseMove(int mouseX, int mouseY) {
        bool widgetStateUpdated = false;
        if (isMouseOnWidget(mouseX, mouseY)) {
            if (widgetState == DEFAULT) {
                widgetState = FOCUS;
                widgetStateUpdated = true;
            }
        } else if (widgetState == FOCUS) {
            widgetState = DEFAULT;
            widgetStateUpdated = true;
        }
        return widgetStateUpdated;
    }

    bool Widget::onMouseMoveEvent(int, int) {
        return true;
    }

    bool Widget::onScroll(double offsetY) {
        bool propagateEvent = true;
        if (isVisible()) {
            propagateEvent = onScrollEvent(offsetY);

            for (const auto& child : children) {
                if (!child->onScroll(offsetY)) {
                    return false;
                }
            }
        }

        if (!propagateEvent) {
            //when a scroll occur: the mouse is implicitly moving relatively to the moved widgets
            onMouseMove(this->mouseX, this->mouseY);
        }

        return propagateEvent;
    }

    bool Widget::onScrollEvent(double) {
        return true;
    }

    void Widget::onResetState() {
        if (isVisible()) {
            handleWidgetResetState();
            onResetStateEvent();

            for (const auto& child : children) {
                child->onResetState();
            }
        }
    }

    void Widget::handleWidgetResetState() {
        if (widgetState == CLICKING) {
            widgetState = FOCUS;
            for (const auto& eventListener : eventListeners) {
                eventListener->onMouseLeftClickRelease(this);
            }
        }

        if (widgetState == FOCUS) {
            widgetState = DEFAULT;
            for (const auto& eventListener : eventListeners) {
                eventListener->onFocusLost(this);
            }
        }
    }

    void Widget::onResetStateEvent() {
        //to override
    }

    int Widget::getMouseX() const {
        return mouseX;
    }

    int Widget::getMouseY() const {
        return mouseY;
    }

    bool Widget::isMouseOnWidget(int mouseX, int mouseY) const {
        Point2 mouseCoordinate(mouseX, mouseY);
        if (widgetRectangle().collideWithPoint(mouseCoordinate)) {
            std::stack<Container*> containers;
            containers.push(getParentContainer());

            while (!containers.empty()) {
                const Container* container = containers.top();
                containers.pop();
                if (container) { //Local variable 'container' may point to invalidated memory
                    if (!container->widgetRectangle().collideWithPoint(mouseCoordinate)) {
                        return false;
                    }
                    containers.push(container->getParentContainer());
                }
            }
            return true;
        }
        return false;
    }

    unsigned int Widget::computeDepthLevel() const {
        unsigned int depthLevel = 0;
        const Widget* currentParent = getParent();
        while (currentParent != nullptr) {
            depthLevel++;
            currentParent = currentParent->getParent();
        }
        return depthLevel;
    }

    void Widget::refreshScissor(bool refreshChildScissor) {
        const Widget *currentWidget = this;
        bool scissorApplied = false;

        while (currentWidget != nullptr) {
            const Container* parentContainer = currentWidget->getParentContainer();
            if (parentContainer && parentContainer->isScrollable()) {
                if (scissorApplied) {
                    throw std::runtime_error("Applied two scissors is not implemented");
                }

                scissorEnabled = true;
                scissorOffset = Vector2<int>((int)parentContainer->getGlobalPositionX(), (int)parentContainer->getGlobalPositionY());
                scissorSize = Vector2<int>((int)parentContainer->getWidth(), (int)parentContainer->getHeight());

                scissorApplied = true;
            }
            currentWidget = parentContainer;
        }

        if (refreshChildScissor) {
            for (const auto& child: children) {
                child->refreshScissor(refreshChildScissor);
            }
        }
    }

    void Widget::prepareRendering(float dt, unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) {
        if (isVisible()) {
            prepareWidgetRendering(dt, renderingOrder, projectionViewMatrix);

            for (const auto& child: children) {
                renderingOrder++;
                child->prepareRendering(dt, renderingOrder, projectionViewMatrix);
            }
        }
    }

    void Widget::prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) {
        //to override
    }

}
