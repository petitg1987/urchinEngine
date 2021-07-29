#include <algorithm>

#include <scene/ui/widget/Widget.h>
#include <scene/ui/widget/container/Container.h>
#include <scene/InputDeviceKey.h>

namespace urchin {

    Widget::Widget(Position position, Size size) :
            i18nService(nullptr),
            renderTarget(nullptr),
            shader(nullptr),
            sceneWidth(0),
            sceneHeight(0),
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

    void Widget::initialize(RenderTarget& renderTarget, const Shader& shader, I18nService& i18nService) {
        this->sceneWidth = renderTarget.getWidth();
        this->sceneHeight = renderTarget.getHeight();

        this->renderTarget = &renderTarget;
        this->shader = &shader;
        this->i18nService = &i18nService;

        createOrUpdateWidget();
        for (auto& child : children) {
            child->initialize(renderTarget, shader, i18nService);
        }
    }

    void Widget::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;
        createOrUpdateWidget();

        for (auto& child : children) {
            child->onResize(sceneWidth, sceneHeight);
        }
    }

    std::shared_ptr<GenericRendererBuilder> Widget::setupUiRenderer(const std::string& name, ShapeType shapeType) const {
        assert(shader);
        assert(renderTarget);

        auto rendererBuilder = GenericRendererBuilder::create(name, *renderTarget, *shader, shapeType);

        //orthogonal matrix with origin at top left screen
        Matrix4<float> projectionMatrix(2.0f / (float)sceneWidth, 0.0f, -1.0f, 0.0f,
                              0.0f, 2.0f / (float)sceneHeight, -1.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f);
        rendererBuilder->addUniformData(sizeof(projectionMatrix), &projectionMatrix); //binding 0

        Vector2<int> translateVector(0, 0);
        rendererBuilder->addUniformData(sizeof(translateVector), &translateVector); //binding 1

        Container* parentContainer = getParentContainer();
        if (parentContainer) {
            Vector2<int> scissorOffset = Vector2<int>(parentContainer->getGlobalPositionX(), parentContainer->getGlobalPositionY());
            Vector2<unsigned int> scissorSize = Vector2<unsigned int>(parentContainer->getWidth(), parentContainer->getHeight());
            rendererBuilder->enableScissor(scissorOffset, scissorSize);
        }

        return rendererBuilder;
    }

    void Widget::updateTranslateVector(GenericRenderer* renderer, const Vector2<int>& translateVector) const {
        renderer->updateUniformData(1, &translateVector);
    }

    RenderTarget& Widget::getRenderTarget() const {
        assert(renderTarget);
        return *renderTarget;
    }

    unsigned int Widget::getSceneWidth() const {
        return sceneWidth;
    }

    unsigned int Widget::getSceneHeight() const {
        return sceneHeight;
    }

    Widget* Widget::getParent() const {
        return parent;
    }

    void Widget::addChild(const std::shared_ptr<Widget>& childWidget) {
        if (childWidget->getParent()) {
            throw std::runtime_error("Cannot add a widget which already have a parent");
        }

        childWidget->parent = this;
        children.push_back(childWidget);

        if (renderTarget) {
            childWidget->initialize(getRenderTarget(), *shader, *i18nService);
        }
    }

    const std::vector<std::shared_ptr<Widget>>& Widget::getChildren() const {
        return children;
    }

    void Widget::detachChild(Widget* childWidget) {
        if (childWidget) {
            auto itFind = std::find_if(children.begin(), children.end(), [&childWidget](const auto &o) { return childWidget == o.get(); });
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
        return widthLengthToPixel(position.getPositionX(), position.getPositionTypeX(), [&](){return (float)getPositionY();});
    }

    /**
    * @return Relative position Y of the widget
    */
    int Widget::getPositionY() const {
        return heightLengthToPixel(position.getPositionY(), position.getPositionTypeY(), [&](){return (float)getPositionX();});
    }

    const WidgetOutline& Widget::getOutline() const {
        return widgetOutline;
    }

    int Widget::getGlobalPositionX() const {
        int startPosition = 0;
        if (parent) {
            if (position.getRelativeTo() == RelativeTo::PARENT_TOP_LEFT || position.getRelativeTo() == RelativeTo::PARENT_BOTTOM_LEFT) {
                startPosition = parent->getGlobalPositionX() + parent->getOutline().leftWidth;
            } else if (position.getRelativeTo() == RelativeTo::PARENT_TOP_RIGHT || position.getRelativeTo() == RelativeTo::PARENT_BOTTOM_RIGHT) {
                startPosition = parent->getGlobalPositionX() - parent->getOutline().rightWidth + (int) parent->getWidth();
            }
        }
        return startPosition + getPositionX();
    }

    int Widget::getGlobalPositionY() const {
        int startPosition = 0;
        if (parent) {
            if (position.getRelativeTo() == RelativeTo::PARENT_TOP_LEFT || position.getRelativeTo() == RelativeTo::PARENT_TOP_RIGHT) {
                startPosition = parent->getGlobalPositionY() + parent->getOutline().topWidth;
            } else if (position.getRelativeTo() == RelativeTo::PARENT_BOTTOM_LEFT || position.getRelativeTo() == RelativeTo::PARENT_BOTTOM_RIGHT) {
                startPosition = parent->getGlobalPositionY() - parent->getOutline().bottomWidth + (int) parent->getHeight();
            }

            auto* scrollable = dynamic_cast<Scrollable*>(parent);
            if (scrollable) {
                startPosition += scrollable->getScrollShiftY();
            }
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
        return (unsigned int)widthLengthToPixel(size.getWidth(), size.getWidthSizeType(), [&](){return (float)getHeight();});
    }

    unsigned int Widget::getHeight() const {
        return (unsigned int)heightLengthToPixel(size.getHeight(), size.getHeightSizeType(), [&](){return (float)getWidth();});
    }

    int Widget::widthLengthToPixel(float widthValue, LengthType lengthType, const std::function<float()>& heightValueInPixel) const {
        if (lengthType == LengthType::SCREEN_PERCENT) {
            return (int)(widthValue / 100.0f * (float)getSceneWidth());
        } else if (lengthType == LengthType::CONTAINER_PERCENT)  {
            return (int)(widthValue / 100.0f * (float)getParentContainer()->getContentWidth());
        } else if (lengthType == LengthType::RELATIVE_LENGTH) {
            float relativeMultiplyFactor = widthValue;
            return (int)(heightValueInPixel() * relativeMultiplyFactor);
        } else if (lengthType == LengthType::PIXEL) {
            return (int)widthValue;
        }
        throw std::runtime_error("Unknown length type: " + std::to_string(lengthType));
    }

    float Widget::widthPixelToLength(float widthPixel, LengthType lengthType) const {
        if (lengthType == LengthType::SCREEN_PERCENT) {
            return (widthPixel / (float)getSceneWidth()) * 100.0f;
        } else if (lengthType == LengthType::CONTAINER_PERCENT) {
            return (widthPixel / (float)getParentContainer()->getContentWidth()) * 100.0f;
        } else if (lengthType == LengthType::PIXEL) {
            return widthPixel;
        }
        throw std::runtime_error("Unknown/unimplemented length type: " + std::to_string(lengthType));
    }

    int Widget::heightLengthToPixel(float heightValue, LengthType lengthType, const std::function<float()>& widthValueInPixel) const {
        if (lengthType == LengthType::SCREEN_PERCENT) {
            return (int)(heightValue / 100.0f * (float)getSceneHeight());
        } else if (lengthType == LengthType::CONTAINER_PERCENT)  {
            return (int)(heightValue / 100.0f * (float)getParentContainer()->getContentHeight());
        } else if (lengthType == LengthType::RELATIVE_LENGTH) {
            float relativeMultiplyFactor = heightValue;
            return (int)(widthValueInPixel() * relativeMultiplyFactor);
        } else if (lengthType == LengthType::PIXEL) {
            return (int)heightValue;
        }
        throw std::runtime_error("Unknown length type: " + std::to_string(lengthType));
    }

    float Widget::heightPixelToLength(float heightPixel, LengthType lengthType) const {
        if (lengthType == LengthType::SCREEN_PERCENT) {
            return (heightPixel / (float)getSceneHeight()) * 100.0f;
        } else if (lengthType == LengthType::CONTAINER_PERCENT) {
            return (heightPixel / (float)getParentContainer()->getContentHeight()) * 100.0f;
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

    bool Widget::onKeyPress(unsigned int key) {
        bool propagateEvent = true;
        if (isVisible()) {
            bool widgetStateUpdated = handleWidgetKeyPress(key);
            propagateEvent = onKeyPressEvent(key);

            if (widgetStateUpdated && widgetState == Widget::CLICKING) {
                for (auto& eventListener : eventListeners) {
                    eventListener->onMouseLeftClick(this);
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
        if (key == InputDeviceKey::MOUSE_LEFT) {
            if (isMouseOnWidget(mouseX, mouseY)) {
                //In some rare cases, the state could be different from FOCUS:
                // - Widget has just been made visible and mouse has not moved yet
                // - UIRenderer has just been enable and mouse has not moved yet
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
                    eventListener->onMouseLeftClickRelease(this);
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
        if (key == InputDeviceKey::MOUSE_LEFT) {
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
                    eventListener->onFocus(this);
                }
            } else if (widgetStateUpdated && widgetState == Widget::DEFAULT) {
                for (auto& eventListener : eventListeners) {
                    eventListener->onFocusLost(this);
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

    bool Widget::isMouseOnWidget(int mouseX, int mouseY) {
        Point2<int> mouseCoordinate(mouseX, mouseY);
        Rectangle<int> widgetZone(
                Point2<int>(getGlobalPositionX(), getGlobalPositionY()),
                Point2<int>(getGlobalPositionX() + (int)getWidth(), getGlobalPositionY() + (int)getHeight()));

        if (widgetZone.collideWithPoint(mouseCoordinate)) {
            std::stack<Container*> containers;
            containers.push(getParentContainer());

            while (!containers.empty()) {
                Container* container = containers.top();
                containers.pop();
                if (container) {
                    Rectangle<int> containerZone(
                            Point2<int>(container->getGlobalPositionX(), container->getGlobalPositionY()),
                            Point2<int>(container->getGlobalPositionX() + (int)container->getContentWidth(), container->getGlobalPositionY() + (int)container->getContentHeight()));

                    if (!containerZone.collideWithPoint(mouseCoordinate)) {
                        return false;
                    }
                    containers.push(container->getParentContainer());
                }
            }
            return true;
        }
        return false;
    }

    void Widget::prepareRendering(float dt) {
        if (isVisible()) {
            prepareWidgetRendering(dt);

            for (auto& child : children) {
                child->prepareRendering(dt);
            }
        }
    }

}
