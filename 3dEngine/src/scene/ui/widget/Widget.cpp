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
        for (auto& child : children) {
            child->initialize(uiRenderer);
        }
    }

    bool Widget::isInitialized() const {
        return uiRenderer != nullptr;
    }

    void Widget::onResize() {
        createOrUpdateWidget();

        for (auto& child : children) {
            child->onResize();
        }
    }

    void Widget::onCameraProjectionUpdate() {
        createOrUpdateWidget();

        for (auto& child : children) {
            child->onCameraProjectionUpdate();
        }
    }

    std::shared_ptr<GenericRendererBuilder> Widget::setupUiRenderer(const std::string& name, ShapeType shapeType, bool enableTransparency) const {
        assert(isInitialized());

        auto rendererBuilder = GenericRendererBuilder::create(name, uiRenderer->getRenderTarget(), uiRenderer->getShader(), shapeType);

        if (!uiRenderer->getUi3dData()) { //UI 2d
            //orthogonal matrix with origin at top left screen
            Matrix4<float> orthogonalProjMatrix(2.0f / (float) uiRenderer->getSceneSize().X, 0.0f, -1.0f, 0.0f,
                                                0.0f, 2.0f / (float) uiRenderer->getSceneSize().Y, -1.0f, 0.0f,
                                                0.0f, 0.0f, 1.0f, 0.0f,
                                                0.0f, 0.0f, 0.0f, 1.0f);
            rendererBuilder->addUniformData(sizeof(orthogonalProjMatrix), &orthogonalProjMatrix); //binding 0
            if (enableTransparency) {
                rendererBuilder->enableTransparency({BlendFunction::buildDefault()});
            }
        } else { //UI 3d
            rendererBuilder->enableDepthTest();
            rendererBuilder->enableDepthWrite(); //TODO avoid z-fighting
            rendererBuilder->addUniformData(sizeof(uiRenderer->getUi3dData()->cameraProjectionMatrix), &uiRenderer->getUi3dData()->cameraProjectionMatrix); //binding 0
            if (enableTransparency) {
                //transparency is currently not supported (only discard in fragment shader is supported)
            }
        }

        rendererBuilder->addUniformData(sizeof(positioningData), &positioningData); //binding 1

        Container* parentContainer = getParentContainer();
        if (parentContainer) {
            Vector2<int> scissorOffset = Vector2<int>(parentContainer->getGlobalPositionX(), parentContainer->getGlobalPositionY());
            Vector2<unsigned int> scissorSize = Vector2<unsigned int>(parentContainer->getWidth(), parentContainer->getHeight());
            rendererBuilder->enableScissor(scissorOffset, scissorSize);
        }

        return rendererBuilder;
    }

    void Widget::updatePositioning(GenericRenderer* renderer, const Matrix4<float>& viewMatrix, const Vector2<int>& translateVector) const {
        if (uiRenderer->getUi3dData()) {
            positioningData.viewModelMatrix = viewMatrix * uiRenderer->getUi3dData()->modelMatrix;
            positioningData.normalMatrix = uiRenderer->getUi3dData()->normalMatrix;
        }
        positioningData.translate = translateVector;
        renderer->updateUniformData(1, &positioningData);
    }

    Point2<unsigned int> Widget::getSceneSize() const {
        if (uiRenderer) {
            return uiRenderer->getSceneSize();
        }
        return Point2<unsigned int>(0, 0);
    }

    I18nService* Widget::getI18nService() const {
        if (uiRenderer) {
            return &uiRenderer->getI18nService();
        }
        return nullptr;
    }

    Widget* Widget::getParent() const {
        return parent;
    }

    Container* Widget::getParentContainer() const {
        Widget *parent = getParent();
        while (parent != nullptr) {
            auto* containerParent = dynamic_cast<Container*>(parent);
            if (containerParent) {
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
            auto itFind = std::find_if(children.begin(), children.end(), [&childWidget](const auto& o) { return childWidget == o.get(); });
            if (itFind == children.end()) {
                throw std::runtime_error("The provided child widget is not a child of this widget");
            }

            childWidget->parent = nullptr;
            children.erase(itFind);
        }
    }

    void Widget::detachChildren() {
        for(auto it = children.begin(); it != children.end();) {
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

    void Widget::updatePosition(Position position) {
        auto* thisContainer = dynamic_cast<Container*>(this);
        if (thisContainer) {
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
        return widthLengthToPixel(position.getX(), position.getXType(), [&](){return (float)getPositionY();});
    }

    /**
    * @return Relative position Y of the widget
    */
    int Widget::getPositionY() const {
        return heightLengthToPixel(position.getY(), position.getYType(), [&](){return (float)getPositionX();});
    }

    const WidgetOutline& Widget::getOutline() const {
        return widgetOutline;
    }

    int Widget::getGlobalPositionX() const {
        int startPosition = 0;
        if (parent) {
            if (position.getRelativeTo() == RelativeTo::PARENT_TOP_LEFT
                    || position.getRelativeTo() == RelativeTo::PARENT_BOTTOM_LEFT
                    || position.getRelativeTo() == RelativeTo::PARENT_CENTER_Y_LEFT) { //left
                startPosition = parent->getGlobalPositionX() + parent->getOutline().leftWidth;
            } else if (position.getRelativeTo() == RelativeTo::PARENT_TOP_RIGHT
                    || position.getRelativeTo() == RelativeTo::PARENT_BOTTOM_RIGHT
                    || position.getRelativeTo() == RelativeTo::PARENT_CENTER_Y_RIGHT) { //right
                startPosition = parent->getGlobalPositionX() - parent->getOutline().rightWidth + (int)parent->getWidth();
            } else if (position.getRelativeTo() == RelativeTo::PARENT_CENTER_XY
                    || position.getRelativeTo() == RelativeTo::PARENT_CENTER_X_TOP
                    || position.getRelativeTo() == RelativeTo::PARENT_CENTER_X_BOTTOM) { //center X
                startPosition = parent->getGlobalPositionX() + parent->getOutline().leftWidth + (int)((float)parent->getWidth() / 2.0f);
            }
        }

        if (position.getReferencePoint() == RefPoint::TOP_RIGHT || position.getReferencePoint() == RefPoint::BOTTOM_RIGHT) { //right
            startPosition -= (int)getWidth();
        } else if (position.getReferencePoint() == RefPoint::CENTER_XY || position.getReferencePoint() == RefPoint::CENTER_X_TOP) { //center X
            startPosition -= (int)((float)getWidth() / 2.0f);
        }

        return startPosition + getPositionX();
    }

    int Widget::getGlobalPositionY() const {
        int startPosition = 0;
        if (parent) {
            if (position.getRelativeTo() == RelativeTo::PARENT_TOP_LEFT
                    || position.getRelativeTo() == RelativeTo::PARENT_TOP_RIGHT
                    || position.getRelativeTo() == RelativeTo::PARENT_CENTER_X_TOP) { //top
                startPosition = parent->getGlobalPositionY() + parent->getOutline().topWidth;
            } else if (position.getRelativeTo() == RelativeTo::PARENT_BOTTOM_LEFT
                    || position.getRelativeTo() == RelativeTo::PARENT_BOTTOM_RIGHT
                    || position.getRelativeTo() == RelativeTo::PARENT_CENTER_X_BOTTOM) { //bottom
                startPosition = parent->getGlobalPositionY() - parent->getOutline().bottomWidth + (int)parent->getHeight();
            } else if (position.getRelativeTo() == RelativeTo::PARENT_CENTER_XY
                    || position.getRelativeTo() == RelativeTo::PARENT_CENTER_Y_LEFT
                    || position.getRelativeTo() == RelativeTo::PARENT_CENTER_Y_RIGHT) { //center Y
                startPosition = parent->getGlobalPositionY() + parent->getOutline().topWidth + (int)((float)parent->getHeight() / 2.0f);
            }

            auto* scrollable = dynamic_cast<Scrollable*>(parent);
            if (scrollable) {
                startPosition += scrollable->getScrollShiftY();
            }
        }

        if (position.getReferencePoint() == RefPoint::BOTTOM_LEFT || position.getReferencePoint() == RefPoint::BOTTOM_RIGHT) { //bottom
            startPosition -= (int)getHeight();
        } else if (position.getReferencePoint() == RefPoint::CENTER_XY ||  position.getReferencePoint() == RefPoint::CENTER_Y_LEFT) { //center Y
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
        return (unsigned int)widthLengthToPixel(size.getWidth(), size.getWidthType(), [&](){return (float)getHeight();});
    }

    unsigned int Widget::getHeight() const {
        return (unsigned int)heightLengthToPixel(size.getHeight(), size.getHeightType(), [&](){return (float)getWidth();});
    }

    Rectangle<int> Widget::widgetRectangle() const {
        return Rectangle<int>(Point2<int>(getGlobalPositionX(), getGlobalPositionY()),
                              Point2<int>(getGlobalPositionX() + (int)getWidth(), getGlobalPositionY() + (int)getHeight()));
    }

    int Widget::widthLengthToPixel(float widthValue, LengthType lengthType, const std::function<float()>& heightValueInPixel) const {
        if (lengthType == LengthType::SCREEN_PERCENT) {
            return MathFunction::roundToInt(widthValue / 100.0f * (float)getSceneSize().X);
        } else if (lengthType == LengthType::CONTAINER_PERCENT)  {
            if (!getParentContainer()) {
                throw std::runtime_error("Missing parent container on the widget");
            }
            return MathFunction::roundToInt(widthValue / 100.0f * (float)getParentContainer()->getWidth());
        } else if (lengthType == LengthType::RATIO_TO_HEIGHT) {
            float relativeMultiplyFactor = widthValue;
            return MathFunction::roundToInt(heightValueInPixel() * relativeMultiplyFactor);
        } else if (lengthType == LengthType::PIXEL) {
            return MathFunction::roundToInt(widthValue);
        }
        throw std::runtime_error("Unknown length type: " + std::to_string(lengthType));
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

    int Widget::heightLengthToPixel(float heightValue, LengthType lengthType, const std::function<float()>& widthValueInPixel) const {
        if (lengthType == LengthType::SCREEN_PERCENT) {
            return MathFunction::roundToInt(heightValue / 100.0f * (float)getSceneSize().Y);
        } else if (lengthType == LengthType::CONTAINER_PERCENT)  {
            if (!getParentContainer()) {
                throw std::runtime_error("Missing parent container on the widget");
            }
            return MathFunction::roundToInt(heightValue / 100.0f * (float)getParentContainer()->getHeight());
        } else if (lengthType == LengthType::RATIO_TO_WIDTH) {
            float relativeMultiplyFactor = heightValue;
            return MathFunction::roundToInt(widthValueInPixel() * relativeMultiplyFactor);
        } else if (lengthType == LengthType::PIXEL) {
            return MathFunction::roundToInt(heightValue);
        }
        throw std::runtime_error("Unknown length type: " + std::to_string(lengthType));
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
                for (auto& eventListener : eventListeners) {
                    propagateEvent &= eventListener->onMouseLeftClick(this);
                }
            }

            //keep a temporary copy of the widgets in case the underlying action goal is to destroy the widgets
            std::vector<std::shared_ptr<Widget>> childrenCopy = children;
            for (auto& child : childrenCopy) {
                if (!child->onKeyPress(key)) {
                    return false;
                }
            }
        }
        return propagateEvent;
    }

    bool Widget::onKeyPressEvent(unsigned int) {
        return true;
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

    bool Widget::onKeyRelease(unsigned int key) {
        bool propagateEvent = true;
        if (isVisible()) {
            bool widgetStateUpdated = handleWidgetKeyRelease(key);
            propagateEvent = onKeyReleaseEvent(key);

            if (widgetStateUpdated && widgetState == Widget::FOCUS) {
                for (auto& eventListener : eventListeners) {
                    propagateEvent &= eventListener->onMouseLeftClickRelease(this);
                }
            }

            //keep a temporary copy of the widgets in case the underlying action goal is to destroy the widgets
            std::vector<std::shared_ptr<Widget>> childrenCopy = children;
            for (auto& child : childrenCopy) {
                if (!child->onKeyRelease(key)) {
                    return false;
                }
            }
        }
        return propagateEvent;
    }

    bool Widget::onKeyReleaseEvent(unsigned int) {
        return true;
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
                widgetState = DEFAULT;
                widgetStateUpdated = true;
            }
        }
        return widgetStateUpdated;
    }

    bool Widget::onChar(char32_t unicodeCharacter) {
        bool propagateEvent = true;
        if (isVisible()) {
            propagateEvent = onCharEvent(unicodeCharacter);

            for (auto& child : children) {
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
                for (auto& eventListener : eventListeners) {
                    propagateEvent &= eventListener->onFocus(this);
                }
            } else if (widgetStateUpdated && widgetState == Widget::DEFAULT) {
                for (auto& eventListener : eventListeners) {
                    propagateEvent &= eventListener->onFocusLost(this);
                }
            }

            for (auto& child : children) {
                if (!child->onMouseMove(mouseX, mouseY)) {
                    return false;
                }
            }
        }
        return propagateEvent;
    }

    bool Widget::onScroll(double offsetY) {
        bool propagateEvent = true;
        if (isVisible()) {
            propagateEvent = onScrollEvent(offsetY);

            for (auto& child : children) {
                if (!child->onScroll(offsetY)) {
                    return false;
                }
            }
        }
        return propagateEvent;
    }

    bool Widget::onMouseMoveEvent(int, int) {
        return true;
    }

    bool Widget::onScrollEvent(double) {
        return true;
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

    int Widget::getMouseX() const {
        return mouseX;
    }

    int Widget::getMouseY() const {
        return mouseY;
    }

    void Widget::onResetState() {
        if (isVisible()) {
            handleWidgetResetState();

            for (auto& child : children) {
                child->onResetState();
            }
        }
    }

    void Widget::handleWidgetResetState() {
        if (widgetState == CLICKING) {
            widgetState = FOCUS;
            for (auto& eventListener : eventListeners) {
                eventListener->onMouseLeftClickRelease(this);
            }
        }

        if (widgetState == FOCUS) {
            widgetState = DEFAULT;
            for (auto& eventListener : eventListeners) {
                eventListener->onFocusLost(this);
            }
        }
    }

    bool Widget::isMouseOnWidget(int mouseX, int mouseY) const {
        Point2<int> mouseCoordinate(mouseX, mouseY);
        if (widgetRectangle().collideWithPoint(mouseCoordinate)) {
            std::stack<Container*> containers;
            containers.push(getParentContainer());

            while (!containers.empty()) {
                Container* container = containers.top();
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

    void Widget::prepareRendering(float dt, unsigned int& renderingOrder, const Matrix4<float>& viewMatrix) {
        if (isVisible()) {
            prepareWidgetRendering(dt, renderingOrder, viewMatrix);

            for (auto& child: children) {
                renderingOrder++;
                child->prepareRendering(dt, renderingOrder, viewMatrix);
            }
        }
    }

    void Widget::prepareWidgetRendering(float, unsigned int&, const Matrix4<float>&) {
        //to override
    }

}
