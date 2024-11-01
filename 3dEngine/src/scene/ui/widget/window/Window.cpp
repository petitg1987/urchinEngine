#include <memory>
#include <utility>
#include <UrchinCommon.h>

#include <scene/ui/widget/window/Window.h>
#include <scene/ui/displayer/WidgetInstanceDisplayer.h>
#include <scene/InputDeviceKey.h>

namespace urchin {

    Window::Window(Position position, Size size, std::string skinName, std::string titleKey) :
            Widget(position, size),
            skinName(std::move(skinName)),
            titleKey(std::move(titleKey)),
            mousePositionX(0),
            mousePositionY(0),
            state(DEFAULT),
            title(nullptr) {

    }

    std::shared_ptr<Window> Window::create(Widget* parent, Position position, Size size, std::string skinName, std::string titleKey) {
        return Widget::create<Window>(new Window(position, size, std::move(skinName), std::move(titleKey)), parent);
    }

    void Window::createOrUpdateWidget() {
        //detach children
        detachChild(title.get());

        //skin information
        auto windowChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "window", UdaAttribute("skin", skinName));

        auto skinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "skin", UdaAttribute(), windowChunk);
        texWindow = UISkinService::instance().createWidgetTexture((unsigned int)getWidth(), (unsigned int)getHeight(), skinChunk, &getOutline());
        changeTexture(texWindow);

        if (!titleKey.empty()) {
            auto textSkinChunk = UISkinService::instance().getSkinReader().getFirstChunk(true, "textSkin", UdaAttribute(), windowChunk);
            title = Text::create(this, Position(0.0f, 0.0f, PIXEL), textSkinChunk->getStringValue(), i18n(titleKey));
            title->updatePosition(Position(0.0f, -((float)getOutline().topWidth + title->getHeight()) / 2.0f, PIXEL));
        }
    }

    bool Window::requireRenderer() const {
        return true;
    }

    WidgetType Window::getWidgetType() const {
        return WidgetType::WINDOW;
    }

    bool Window::onKeyPressEvent(InputDeviceKey key) {
        bool propagateEvent = true;
        if (key == InputDeviceKey::MOUSE_LEFT) {
            Rectangle2D titleZone(Point2((int)getGlobalPositionX(), (int)getGlobalPositionY()),
                                  Point2((int)getGlobalPositionX() + ((int) getWidth() - getOutline().rightWidth), (int)getGlobalPositionY() + getOutline().topWidth));
            Rectangle2D closeZone(Point2((int)getGlobalPositionX() + ((int) getWidth() - getOutline().rightWidth), (int)getGlobalPositionY()),
                                  Point2((int)getGlobalPositionX() + (int) getWidth(), (int)getGlobalPositionY() + getOutline().topWidth));

            if (!getUi3dData() && titleZone.collideWithPoint(Point2<int>(getMouseX(), getMouseY()))) {
                mousePositionX = getMouseX() - MathFunction::roundToInt(getPositionX());
                mousePositionY = getMouseY() - MathFunction::roundToInt(getPositionY());
                state = MOVING;
            }

            if (closeZone.collideWithPoint(Point2(getMouseX(), getMouseY()))) {
                state = CLOSING;
            }

            if (widgetRectangle().collideWithPoint(Point2(getMouseX(), getMouseY()))) {
                notifyObservers(this, SET_IN_FOREGROUND);
                propagateEvent = false;
            }
        }

        return propagateEvent;
    }

    bool Window::onKeyReleaseEvent(InputDeviceKey key) {
        Rectangle2D closeZone(Point2((int)getGlobalPositionX() + ((int)getWidth() - getOutline().rightWidth), (int)getGlobalPositionY()),
                              Point2((int)getGlobalPositionX() + (int)getWidth(), (int)getGlobalPositionY() + getOutline().topWidth));
        if (key == InputDeviceKey::MOUSE_LEFT && state == CLOSING && closeZone.collideWithPoint(Point2(getMouseX(), getMouseY()))) {
            setIsVisible(false);
        }

        state = DEFAULT;

        return true;
    }

    bool Window::onMouseMoveEvent(int mouseX, int mouseY) {
        bool propagateEvent = true;
        if (state == MOVING) {
            auto positionPixelX = (float)(mouseX - mousePositionX);
            auto positionLengthX = widthPixelToLength(positionPixelX, getPosition().getXType());

            auto positionPixelY = (float)(mouseY - mousePositionY);
            auto positionLengthY = heightPixelToLength(positionPixelY, getPosition().getYType());

            updatePosition(Position(positionLengthX, getPosition().getXType(),
                                    positionLengthY, getPosition().getYType(),
                                    getPosition().getRelativeTo(), getPosition().getReferencePoint()));
            propagateEvent = false;
        } else if (widgetRectangle().collideWithPoint(Point2(mouseX, mouseY))) {
            propagateEvent = false;
        }

        return propagateEvent;
    }

    void Window::onResetStateEvent() {
        state = DEFAULT;
    }

}
