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
    }

    bool Widget::isInitialized() const {
        return uiRenderer != nullptr;
    }

    void Widget::onResize() {
        createOrUpdateWidget();

        for (const auto& child : children) {
            child->onResize();
        }
    }

    void Widget::onCameraProjectionUpdate() {
        createOrUpdateWidget();

        for (const auto& child : children) {
            child->onCameraProjectionUpdate();
        }
    }

    std::shared_ptr<GenericRendererBuilder> Widget::setupUiRenderer(std::string name, ShapeType shapeType, bool hasTransparency) const {
        assert(isInitialized());
        auto rendererBuilder = GenericRendererBuilder::create(std::move(name), uiRenderer->getRenderTarget(), uiRenderer->getShader(), shapeType);

        Matrix4<float> normalMatrix, projectionViewModelMatrix;
        if (uiRenderer->getUi3dData()) {
            rendererBuilder->enableDepthTest();
            rendererBuilder->enableDepthWrite();
            rendererBuilder->enableTransparency({ //always active transparency to ensure that emissive factor stay unchanged (see fragment shader for more details)
                BlendFunction::build(BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::ZERO, BlendFactor::ONE),
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

        const Container* parentContainer = getParentContainer();
        if (parentContainer && !uiRenderer->getUi3dData() /* scissor test is not functional for UI 3d */) {
            Vector2<int> scissorOffset(parentContainer->getGlobalPositionX(), parentContainer->getGlobalPositionY());
            Vector2<unsigned int> scissorSize(parentContainer->getWidth(), parentContainer->getHeight());
            rendererBuilder->enableScissor(scissorOffset, scissorSize);
        }

        return rendererBuilder;
    }

    void Widget::updatePositioning(GenericRenderer* renderer, const Matrix4<float>& projectionViewMatrix, const Vector2<int>& translateVector) const {
        Matrix4<float> projectionViewModelMatrix;

        if (uiRenderer->getUi3dData()) {
            float zBias = (float)computeDepthLevel() * 0.003f;
            Matrix4<float> translateMatrix;
            translateMatrix.buildTranslation((float)translateVector.X, (float)translateVector.Y, zBias);
            projectionViewModelMatrix = projectionViewMatrix * uiRenderer->getUi3dData()->modelMatrix * translateMatrix;
        } else {
            Matrix4<float> translateMatrix;
            translateMatrix.buildTranslation((float)translateVector.X, (float)translateVector.Y, 0.0f);

            Matrix4<float> orthogonalMatrix( //orthogonal matrix with origin at top left screen
                    2.0f / (float) uiRenderer->getUiResolution().X, 0.0f, -1.0f, 0.0f,
                    0.0f, 2.0f / (float) uiRenderer->getUiResolution().Y, -1.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
            projectionViewModelMatrix = orthogonalMatrix * translateMatrix;
        }

        renderer->updateUniformData(1, &projectionViewModelMatrix);
    }

    I18nService* Widget::getI18nService() const {
        if (!uiRenderer) {
            throw std::runtime_error("Internationalization service not available because the widget is not initialized");
        }
        return &uiRenderer->getI18nService();
    }

    UI3dData* Widget::getUi3dData() const {
        if (!uiRenderer) {
            throw std::runtime_error("UI 3d data not available because the widget is not initialized");
        }
        return uiRenderer->getUi3dData();
    }

    Widget* Widget::getParent() const {
        return parent;
    }

    Container* Widget::getParentContainer() const {
        Widget *parent = getParent();
        while (parent != nullptr) {
            if (auto* containerParent = dynamic_cast<Container*>(parent)) {
                return containerParent;
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

    Widget::WidgetStates Widget::getWidgetState() const {
        return widgetState;
    }

    Point2<unsigned int> Widget::getSceneSize() const {
        if (!uiRenderer) {
            throw std::runtime_error("Scene size not available because the widget is not initialized");
        }
        return uiRenderer->getUiResolution();
    }

    void Widget::updatePosition(Position position) {
        if (dynamic_cast<Container*>(this)) {
            throw std::runtime_error("Can not move a container: scissor update is not implemented");
        }
        this->position = position;
    }

    Position Widget::getPosition() const {
        return position;
    }

    /**
    * @return Relative position X of the widget
    */
    int Widget::getPositionX() const {
        return widthLengthToPixel(position.getX(), position.getXType(), [&](){ return (float)getPositionY(); });
    }

    /**
    * @return Relative position Y of the widget
    */
    int Widget::getPositionY() const {
        return heightLengthToPixel(position.getY(), position.getYType(), [&](){ return (float)getPositionX(); });
    }

    const WidgetOutline& Widget::getOutline() const {
        return widgetOutline;
    }

    int Widget::getGlobalPositionX() const {
        int startPosition = 0;
        if (position.getRelativeTo() == RelativeTo::PARENT_LEFT_TOP
                || position.getRelativeTo() == RelativeTo::PARENT_LEFT_BOTTOM
                || position.getRelativeTo() == RelativeTo::PARENT_LEFT_CENTERY) { //left
            if (parent) {
                startPosition = parent->getGlobalPositionX() + parent->getOutline().leftWidth;
            } else {
                startPosition = 0;
            }
        } else if (position.getRelativeTo() == RelativeTo::PARENT_RIGHT_TOP
                || position.getRelativeTo() == RelativeTo::PARENT_RIGHT_BOTTOM
                || position.getRelativeTo() == RelativeTo::PARENT_RIGHT_CENTERY) { //right
            if (parent) {
                startPosition = parent->getGlobalPositionX() - parent->getOutline().rightWidth + (int) parent->getWidth();
            } else {
                startPosition = (int)getSceneSize().X;
            }
        } else if (position.getRelativeTo() == RelativeTo::PARENT_CENTER_XY
                || position.getRelativeTo() == RelativeTo::PARENT_CENTERX_TOP
                || position.getRelativeTo() == RelativeTo::PARENT_CENTERX_BOTTOM) { //center X
            if (parent) {
                startPosition = parent->getGlobalPositionX() + parent->getOutline().leftWidth + (int) ((float) parent->getWidth() / 2.0f);
            } else {
                startPosition = (int)((float)getSceneSize().X / 2.0f);
            }
        }

        if (position.getReferencePoint() == RefPoint::RIGHT_TOP || position.getReferencePoint() == RefPoint::RIGHT_BOTTOM || position.getReferencePoint() == RefPoint::RIGHT_CENTERY) { //right
            startPosition -= (int)getWidth();
        } else if (position.getReferencePoint() == RefPoint::CENTER_XY || position.getReferencePoint() == RefPoint::CENTERX_TOP || position.getReferencePoint() == RefPoint::CENTERX_BOTTOM) { //center X
            startPosition -= (int)((float)getWidth() / 2.0f);
        }

        return startPosition + getPositionX();
    }

    int Widget::getGlobalPositionY() const {
        int startPosition = 0;
        if (position.getRelativeTo() == RelativeTo::PARENT_LEFT_TOP
                || position.getRelativeTo() == RelativeTo::PARENT_RIGHT_TOP
                || position.getRelativeTo() == RelativeTo::PARENT_CENTERX_TOP) { //top
            if (parent) {
                startPosition = parent->getGlobalPositionY() + parent->getOutline().topWidth;
            } else {
                startPosition = 0;
            }
        } else if (position.getRelativeTo() == RelativeTo::PARENT_LEFT_BOTTOM
                || position.getRelativeTo() == RelativeTo::PARENT_RIGHT_BOTTOM
                || position.getRelativeTo() == RelativeTo::PARENT_CENTERX_BOTTOM) { //bottom
            if (parent) {
                startPosition = parent->getGlobalPositionY() - parent->getOutline().bottomWidth + (int) parent->getHeight();
            } else {
                startPosition = (int)getSceneSize().Y;
            }
        } else if (position.getRelativeTo() == RelativeTo::PARENT_CENTER_XY
                || position.getRelativeTo() == RelativeTo::PARENT_LEFT_CENTERY
                || position.getRelativeTo() == RelativeTo::PARENT_RIGHT_CENTERY) { //center Y
            if (parent) {
                startPosition = parent->getGlobalPositionY() + parent->getOutline().topWidth + (int) ((float) parent->getHeight() / 2.0f);
            } else {
                startPosition = (int)((float)getSceneSize().Y / 2.0f);
            }
        }

        if (const auto* scrollable = dynamic_cast<Scrollable*>(parent)) {
            startPosition += scrollable->getScrollShiftY();
        }

        if (position.getReferencePoint() == RefPoint::LEFT_BOTTOM || position.getReferencePoint() == RefPoint::RIGHT_BOTTOM || position.getReferencePoint() == RefPoint::CENTERX_BOTTOM) { //bottom
            startPosition -= (int)getHeight();
        } else if (position.getReferencePoint() == RefPoint::CENTER_XY ||  position.getReferencePoint() == RefPoint::LEFT_CENTERY || position.getReferencePoint() == RefPoint::RIGHT_CENTERY) { //center Y
            startPosition -= (int)((float)getHeight() / 2.0f);
        }

        return startPosition + getPositionY();
    }

    void Widget::updateSize(Size size) {
        setSize(size);
        createOrUpdateWidget();
    }

    void Widget::setSize(Size size) {
        this->size = size;
    }

    Size Widget::getSize() const {
        return size;
    }

    unsigned int Widget::getWidth() const {
        return (unsigned int)widthLengthToPixel(size.getWidth(), size.getWidthType(), [&](){ return (float)getHeight(); });
    }

    unsigned int Widget::getHeight() const {
        return (unsigned int)heightLengthToPixel(size.getHeight(), size.getHeightType(), [&](){ return (float)getWidth(); });
    }

    Rectangle2D<int> Widget::widgetRectangle() const {
        return Rectangle2D<int>(Point2<int>(getGlobalPositionX(), getGlobalPositionY()),
                                Point2<int>(getGlobalPositionX() + (int)getWidth(), getGlobalPositionY() + (int)getHeight()));
    }

    float Widget::widthPixelToLength(float widthPixel, LengthType lengthType) const {
        if (lengthType == LengthType::SCREEN_PERCENT) {
            return (widthPixel / (float)getSceneSize().X) * 100.0f;
        } else if (lengthType == LengthType::CONTAINER_PERCENT) {
            return (widthPixel / (float)getParentContainer()->getWidth()) * 100.0f;
        } else if (lengthType == LengthType::PIXEL) {
            return widthPixel;
        }
        throw std::runtime_error("Unknown/unimplemented length type: " + std::to_string(lengthType));
    }

    float Widget::heightPixelToLength(float heightPixel, LengthType lengthType) const {
        if (lengthType == LengthType::SCREEN_PERCENT) {
            return (heightPixel / (float)getSceneSize().Y) * 100.0f;
        } else if (lengthType == LengthType::CONTAINER_PERCENT) {
            return (heightPixel / (float)getParentContainer()->getHeight()) * 100.0f;
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

    bool Widget::onKeyPress(unsigned int key) {
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

    bool Widget::handleWidgetKeyPress(unsigned int key) {
        bool widgetStateUpdated = false;
        if (key == (int)InputDeviceKey::MOUSE_LEFT) {
            if (isMouseOnWidget(mouseX, mouseY)) {
                //In some rare cases, the state could be different from FOCUS:
                // - Widget has just been made visible and mouse has not moved yet
                // - UIRenderer has just been enabled and mouse has not moved yet
                if (widgetState == FOCUS) {
                    widgetState = CLICKING;
                    widgetStateUpdated = true;
                }
            }
        }
        return widgetStateUpdated;
    }

    bool Widget::onKeyPressEvent(unsigned int) {
        return true;
    }

    bool Widget::onKeyRelease(unsigned int key) {
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

    bool Widget::handleWidgetKeyRelease(unsigned int key) {
        bool widgetStateUpdated = false;
        if (key == (int)InputDeviceKey::MOUSE_LEFT) {
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

    bool Widget::onKeyReleaseEvent(unsigned int) {
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
        Point2<int> mouseCoordinate(mouseX, mouseY);
        if (widgetRectangle().collideWithPoint(mouseCoordinate)) {
            std::stack<Container*> containers;
            containers.push(getParentContainer());

            while (!containers.empty()) {
                const Container* container = containers.top();
                containers.pop();
                if (container) {
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
