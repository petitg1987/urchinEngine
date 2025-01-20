#include <algorithm>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/container/Container.h>
#include <scene/ui/widget/window/Window.h>
#include <scene/ui/widget/WidgetTypeUtil.h>
#include <scene/ui/displayer/WidgetSetDisplayer.h>
#include <scene/ui/UIRenderer.h>
#include <scene/InputDeviceKey.h>

namespace urchin {

    Widget::Widget(Position position, Size size) :
            uiRenderer(nullptr),
            mouseX(0),
            mouseY(0),
            parent(nullptr),
            widgetState(DEFAULT),
            position(std::move(position)),
            size(size),
            scale(Vector2(1.0f, 1.0f)),
            rotationZ(0.0f),
            alphaFactor(1.0f),
            bIsVisible(true) {

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

    void Widget::uninitialize() {
        if (this->uiRenderer) {
            this->uiRenderer->getWidgetSetDisplayer().removeWidget(this);
        }
        this->uiRenderer = nullptr;

        for (const auto& child : children) {
            child->uninitialize();
        }
    }

    bool Widget::isInitialized() const {
        return uiRenderer != nullptr;
    }

    void Widget::onResize() {
        createOrUpdateWidget();
        notifyObservers(this, SIZE_UPDATED);

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

    const UIRenderer& Widget::getUiRenderer() const {
        if (!uiRenderer) {
            throw std::runtime_error("UI renderer not available because the widget is not initialized");
        }
        return *uiRenderer;
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

    std::string Widget::getName() const {
        WidgetType widgetType = getWidgetType();
        std::string name = WidgetTypeUtil::toWidgetTypeString(widgetType);
        if (widgetType == WidgetType::TEXT) {
            std::string inputText = static_cast<const Text*>(this)->getInputText().getText();
            name += "_" + inputText.substr(0, std::min((std::size_t)10, inputText.size()));
        } else if (getTexture()) {
            name += "_" + FileUtil::getFileNameNoExtension(getTexture()->getName());
        }
        return name;
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

    bool Widget::isRootWidget() const {
        return getParent() == nullptr || getWidgetType() == WidgetType::WINDOW;
    }

    const Widget* Widget::getParentRootWidget() const {
        auto currentWidget = this;
        do {
            if (currentWidget->isRootWidget()) {
                return currentWidget;
            }
            currentWidget = currentWidget->getParent();
        } while (currentWidget != nullptr);

        throw std::runtime_error("Root widget not found on: " + getName());
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

    bool Widget::isUi3D() const {
       return getUi3dData() != nullptr;
    }

    bool Widget::requireRenderer() const {
        return false;
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

            if (uiRenderer) {
                uiRenderer->getWidgetSetDisplayer().removeWidget(childWidget);
            }
            childWidget->parent = nullptr;
            children.erase(itFind);
        }
    }

    void Widget::detachChildren() {
        for (auto it = children.begin(); it != children.end();) {
            if (uiRenderer) {
                uiRenderer->getWidgetSetDisplayer().removeWidget(it->get());
            }
            (*it)->parent = nullptr;
            it = children.erase(it);
        }
    }

    void Widget::clearEventListeners() {
        eventListeners.clear();
    }

    void Widget::addEventListener(std::shared_ptr<EventListener> eventListener) {
        this->eventListeners.push_back(std::move(eventListener));
        this->eventListeners.back()->onInit(this);
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
        this->position = std::move(position);
        onPositionUpdated();
    }

    void Widget::onPositionUpdated() {
        notifyObservers(this, POSITION_UPDATED);
        for (const auto& child : children) {
            child->onPositionUpdated();
        }
    }

    Position Widget::getPosition() const {
        return position;
    }

    /**
    * @return Relative position X of the widget
    */
    float Widget::getPositionX() const {
        return widthLengthToPixel(position.getX(), position.getXType(), [this] { return getPositionY(); });
    }

    /**
    * @return Relative position Y of the widget
    */
    float Widget::getPositionY() const {
        return heightLengthToPixel(position.getY(), position.getYType(), [this] { return getPositionX(); });
    }

    float Widget::getGlobalPositionX() const {
        float startPosition = 0;
        if (position.getRelativeTo() == PARENT_LEFT_TOP
                || position.getRelativeTo() == PARENT_LEFT_BOTTOM
                || position.getRelativeTo() == PARENT_LEFT_CENTERY) { //left
            if (parent) {
                startPosition = parent->getGlobalPositionX() + (float)parent->getOutline().leftWidth;
            } else {
                startPosition = 0.0f;
            }
        } else if (position.getRelativeTo() == PARENT_RIGHT_TOP
                || position.getRelativeTo() == PARENT_RIGHT_BOTTOM
                || position.getRelativeTo() == PARENT_RIGHT_CENTERY) { //right
            if (parent) {
                startPosition = parent->getGlobalPositionX() + parent->getWidth() - (float)parent->getOutline().rightWidth ;
            } else {
                startPosition = (float)getSceneSize().X;
            }
        } else if (position.getRelativeTo() == PARENT_CENTER_XY
                || position.getRelativeTo() == PARENT_CENTERX_TOP
                || position.getRelativeTo() == PARENT_CENTERX_BOTTOM) { //center X
            if (parent) {
                startPosition = parent->getGlobalPositionX() + (float)parent->getOutline().leftWidth + parent->getWidthMinusOutline() / 2.0f;
            } else {
                startPosition = (float)getSceneSize().X / 2.0f;
            }
        }

        if (position.getReferencePoint() == RIGHT_TOP || position.getReferencePoint() == RIGHT_BOTTOM || position.getReferencePoint() == RIGHT_CENTERY) { //right
            startPosition -= getWidth();
        } else if (position.getReferencePoint() == CENTER_XY || position.getReferencePoint() == CENTERX_TOP || position.getReferencePoint() == CENTERX_BOTTOM) { //center X
            startPosition -= getWidth() / 2.0f;
        }

        return startPosition + getPositionX();
    }

    float Widget::getGlobalPositionY() const {
        float startPosition = 0;
        if (position.getRelativeTo() == PARENT_LEFT_TOP
                || position.getRelativeTo() == PARENT_RIGHT_TOP
                || position.getRelativeTo() == PARENT_CENTERX_TOP) { //top
            if (parent) {
                startPosition = parent->getGlobalPositionY() + (float)parent->getOutline().topWidth;
            } else {
                startPosition = 0.0f;
            }
        } else if (position.getRelativeTo() == PARENT_LEFT_BOTTOM
                || position.getRelativeTo() == PARENT_RIGHT_BOTTOM
                || position.getRelativeTo() == PARENT_CENTERX_BOTTOM) { //bottom
            if (parent) {
                startPosition = parent->getGlobalPositionY() + parent->getHeight() - (float)parent->getOutline().bottomWidth;
            } else {
                startPosition = (float)getSceneSize().Y;
            }
        } else if (position.getRelativeTo() == PARENT_CENTER_XY
                || position.getRelativeTo() == PARENT_LEFT_CENTERY
                || position.getRelativeTo() == PARENT_RIGHT_CENTERY) { //center Y
            if (parent) {
                startPosition = parent->getGlobalPositionY() + (float)parent->getOutline().topWidth + parent->getHeightMinusOutline() / 2.0f;
            } else {
                startPosition = (float)getSceneSize().Y / 2.0f;
            }
        }

        if (const auto* scrollable = dynamic_cast<Scrollable*>(parent)) {
            startPosition += (float)scrollable->getScrollShiftY();
        }

        if (position.getReferencePoint() == LEFT_BOTTOM || position.getReferencePoint() == RIGHT_BOTTOM || position.getReferencePoint() == CENTERX_BOTTOM) { //bottom
            startPosition -= getHeight();
        } else if (position.getReferencePoint() == CENTER_XY ||  position.getReferencePoint() == LEFT_CENTERY || position.getReferencePoint() == RIGHT_CENTERY) { //center Y
            startPosition -= getHeight() / 2.0f;
        }

        return startPosition + getPositionY();
    }

    void Widget::updateSize(Size size) {
        this->size = size;
        notifyObservers(this, SIZE_UPDATED);
    }

    Size Widget::getSize() const {
        return size;
    }

    void Widget::changeTexture(std::shared_ptr<Texture> texture) {
        this->texture = std::move(texture);
        notifyObservers(this, TEXTURE_UPDATED);
    }

    WidgetOutline& Widget::getOutline() {
        return widgetOutline;
    }

    const WidgetOutline& Widget::getOutline() const {
        return widgetOutline;
    }

    float Widget::getWidth() const {
        return widthLengthToPixel(size.getWidth(), size.getWidthType(), [this] { return getHeight(); });
    }

    float Widget::getWidthMinusOutline() const {
        return getWidth() - (float)getOutline().leftWidth - (float)getOutline().rightWidth;
    }

    float Widget::getHeight() const {
        return heightLengthToPixel(size.getHeight(), size.getHeightType(), [this] { return getWidth(); });
    }

    float Widget::getHeightMinusOutline() const {
        return getHeight() - (float)getOutline().topWidth - (float)getOutline().bottomWidth;
    }

    Rectangle2D<int> Widget::widgetRectangle() const {
        return {Point2((int)getGlobalPositionX(), (int)getGlobalPositionY()),
            Point2((int)getGlobalPositionX() + (int)getWidth(), (int)getGlobalPositionY() + (int)getHeight())};
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
        notifyObservers(this, ALPHA_FACTOR_UPDATED);
    }

    float Widget::getAlphaFactor() const {
        return alphaFactor;
    }

    const std::shared_ptr<Texture>& Widget::getTexture() const {
        return texture;
    }

    void Widget::retrieveVertexCoordinates(std::vector<Point2<float>>& vertexCoord) {
        assert(vertexCoord.empty());
        vertexCoord.reserve(6);

        vertexCoord.emplace_back(0.0f, 0.0f);
        vertexCoord.emplace_back(getWidth(), 0.0f);
        vertexCoord.emplace_back(getWidth(), getHeight());

        vertexCoord.emplace_back(0.0f, 0.0f);
        vertexCoord.emplace_back(getWidth(), getHeight());
        vertexCoord.emplace_back(0.0f, getHeight());
    }

    void Widget::retrieveTextureCoordinates(std::vector<Point2<float>>& textureCoord) {
        assert(textureCoord.empty());
        textureCoord.reserve(6);

        textureCoord.emplace_back(0.0f, 0.0f);
        textureCoord.emplace_back(1.0f, 0.0f);
        textureCoord.emplace_back(1.0f, 1.0f);

        textureCoord.emplace_back(0.0f, 0.0f);
        textureCoord.emplace_back(1.0f, 1.0f);
        textureCoord.emplace_back(0.0f, 1.0f);
    }

    std::optional<Scissor> Widget::retrieveScissor() const {
        auto currentWidget = this;
        const Widget *scissorContainer = nullptr;

        while (currentWidget != nullptr) {
            const Container* parentContainer = currentWidget->getParentContainer();
            if (parentContainer && parentContainer->isScrollable()) {
                if (scissorContainer) {
                    throw std::runtime_error("Applied two scissors is not implemented");
                }
                scissorContainer = parentContainer;
            }
            currentWidget = parentContainer;
        }

        if (scissorContainer) [[unlikely]] {
            return Scissor(
                    Vector2((int)scissorContainer->getGlobalPositionX(), (int)scissorContainer->getGlobalPositionY()),
                    Vector2((int)scissorContainer->getWidth(), (int)scissorContainer->getHeight())
            );
        }
        return std::nullopt;
    }

    float Widget::widthPixelToLength(float widthPixel, LengthType lengthType) const {
        if (lengthType == SCREEN_PERCENT) {
            return (widthPixel / (float)getSceneSize().X) * 100.0f;
        } else if (lengthType == CONTAINER_PERCENT) {
            return (widthPixel / getParentContainer()->getWidthMinusOutline()) * 100.0f;
        } else if (lengthType == PARENT_PERCENT) {
            return (widthPixel / getParent()->getWidthMinusOutline()) * 100.0f;
        } else if (lengthType == PIXEL) {
            return widthPixel;
        }
        throw std::runtime_error("Unknown/unimplemented length type: " + std::to_string(lengthType));
    }

    float Widget::heightPixelToLength(float heightPixel, LengthType lengthType) const {
        if (lengthType == SCREEN_PERCENT) {
            return (heightPixel / (float)getSceneSize().Y) * 100.0f;
        } else if (lengthType == CONTAINER_PERCENT) {
            const Container* parentContainer = getParentContainer();
            return (heightPixel / parentContainer->getHeightMinusOutline()) * 100.0f;
        } else if (lengthType == PARENT_PERCENT) {
            return (heightPixel / getParent()->getHeightMinusOutline()) * 100.0f;
        } else if (lengthType == PIXEL) {
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
        if (isInitialized() && isVisible()) {
            bool widgetStateUpdated = handleWidgetKeyPress(key);
            propagateEvent = onKeyPressEvent(key);

            if (widgetStateUpdated && widgetState == CLICKING) {
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
        if (isInitialized() && isVisible()) {
            bool widgetStateUpdated = handleWidgetKeyRelease(key);
            propagateEvent = onKeyReleaseEvent(key);

            if (widgetStateUpdated && widgetState == FOCUS) {
                for (const auto& eventListener : eventListeners) {
                    propagateEvent &= eventListener->onMouseLeftClickRelease(this);
                }
            } else if (widgetStateUpdated && widgetState == DEFAULT) {
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
        if (isInitialized() && isVisible()) {
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
        if (isInitialized() && isVisible()) {
            bool widgetStateUpdated = handleWidgetMouseMove(mouseX, mouseY);
            propagateEvent = onMouseMoveEvent(mouseX, mouseY);

            if (widgetStateUpdated && widgetState == FOCUS) {
                for (const auto& eventListener : eventListeners) {
                    propagateEvent &= eventListener->onFocus(this);
                }
            } else if (widgetStateUpdated && widgetState == DEFAULT) {
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
        if (isInitialized() && isVisible()) {
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

            //keep a temporary copy of the widgets in case the underlying action goal is to destroy the widgets
            std::vector<std::shared_ptr<Widget>> childrenCopy = children;
            for (const auto& child : childrenCopy) {
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

    void Widget::prepareWidgetRendering(float) {
        //to override
    }

}
